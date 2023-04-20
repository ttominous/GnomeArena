#include "MindComponent.h"
#include "CameraManagerComponent.h"
#include "DualAxisCreature.h"
#include "Creature.h"
#include "CreatureSpec.h"
#include "CreatureAnimInstance.h"
#include "CollisionTools.h"
#include "ComplexGameState.h"
#include "MathTools.h"
#include "Debug.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "NavLinkCustomInterface.h"


UMindComponent::UMindComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UMindComponent::init() {
	initialized = true;

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (gameState) {
		debugPathing = gameState->gameConfiguration->debugPathing;
	}

	initAIPerception();
}

void UMindComponent::initAIPerception() {
	if (perception == nullptr) {
		perception = Cast< UAIPerceptionComponent >(GetOwner()->GetComponentByClass(UAIPerceptionComponent::StaticClass()));
		if (perception == nullptr) {
			perception = NewObject< UAIPerceptionComponent >(GetOwner());
			perception->RegisterComponent();

			sight = NewObject< UAISenseConfig_Sight >(perception);
			sight->SightRadius = 1000.0f;
			sight->LoseSightRadius = 2000.0f;
			sight->PeripheralVisionAngleDegrees = 70.0f;
			sight->SetMaxAge(30.0f);
			sight->DetectionByAffiliation.bDetectEnemies = true;
			sight->DetectionByAffiliation.bDetectFriendlies = true;
			sight->DetectionByAffiliation.bDetectNeutrals = true;
			perception->ConfigureSense(*sight);
			perception->SetDominantSense(UAISense_Sight::StaticClass());
		}
		else {
			sight = Cast<UAISenseConfig_Sight>(perception->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()) );
		}
	}
}

void UMindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (jumpCooldown > 0.0f) {
		jumpCooldown -= DeltaTime;
	}

	if (GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* creature = Cast< ACreature >(GetOwner());
		if (!creature->isBot || !creature->IsLocallyControlled() || creature->dead) {
			return;
		}

		if (delay > 0.0f) {
			delay -= DeltaTime;
			return;
		}

		if (!initialized) {
			init();
		}

		update(creature, DeltaTime);
	}

}


void UMindComponent::update(ACreature* creature, float deltaTime) {

	if (creature->IsA(ADualAxisCreature::StaticClass())) {
		ADualAxisCreature* dualAxisCreature = Cast< ADualAxisCreature >(creature);
		dualAxisCreature->aimAtPoint = creature->GetMesh()->GetSocketLocation("Spine") + dualAxisCreature->controlRotation.RotateVector(FVector(500.0f, 0.0f, 0.0));
	}

}


void UMindComponent::findPath(ACreature* creature, FVector targetPosition, float deltaTime) {

	pathRefreshTimer -= deltaTime;
	if (pathRefreshTimer <= 0.0f) {
		pathRefreshTimer = 1.0f;
	}
	else {
		return;
	}

	FNavAgentProperties navAgentProperties = getNavProperties();
	UNavigationSystemV1* navigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	ANavigationData* navigationData = navigationSystem->GetNavDataForProps(navAgentProperties);
	if (navigationData) {
		TSubclassOf<UNavigationQueryFilter> filterClass = UNavigationQueryFilter::StaticClass();
		FSharedConstNavQueryFilter queryFilter = UNavigationQueryFilter::GetQueryFilter(*navigationData, filterClass);

		FVector startPosition = creature->GetActorLocation();
		startPosition.Z -= creature->getCapsuleHalfHeight();
		FPathFindingQuery query = FPathFindingQuery(this, *navigationData, startPosition, targetPosition, queryFilter);

		if (!pathFindingDelegateBound) {
			pathFindingCompletedDelegate.BindUObject(this, &UMindComponent::finishPathFinding);
			pathFindingDelegateBound = true;
		}

		FNavLocation correctedEndLocation;
		if (navigationSystem->ProjectPointToNavigation(targetPosition, correctedEndLocation, FVector(150.0f, 150.0f, 50.0f))) {
			targetPosition = correctedEndLocation.Location;
			query = FPathFindingQuery(this, *navigationData, startPosition, targetPosition, queryFilter);
		}

		navigationSystem->FindPathAsync(navAgentProperties, query, pathFindingCompletedDelegate, EPathFindingMode::Regular);
	}

}


void UMindComponent::finishPathFinding(uint32 pathID, ENavigationQueryResult::Type resultType, FNavPathSharedPtr path) {
	currentPath.Empty();

	UNavigationSystemV1* navigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FNavigationPath* navigationPath = path.Get();
	if (navigationPath != nullptr) {
		TArray < FNavPathPoint > pathPoints = navigationPath->GetPathPoints();
		if (pathPoints.Num() > 0) {
			FVector currentPosition = GetOwner()->GetActorLocation();

			FVector startingDirection = currentPosition - pathPoints[0].Location;
			startingDirection.Z = 0.0f;
			startingDirection.Normalize();

			bool removeSameDirection = true;
			for (int i = 0; i < pathPoints.Num(); i++) {

				if (i > 0) {
					UComplexNavLinkComponent* complexLink = Cast< UComplexNavLinkComponent >(navigationSystem->GetCustomLink(pathPoints[i-1].CustomLinkId));
					if (complexLink) {
						FNavPathNode node;
						node.location = pathPoints[i].Location;
						node.link = complexLink;
						currentPath.Add(node);
						removeSameDirection = false;
						continue;
					}
				}

				/*if (i <= 2 && removeSameDirection) {
					FVector currentDirection = pathPoints[i].Location - pathPoints[0].Location;
					currentDirection.Z = 0.0f;
					currentDirection.Normalize();

					if (FVector::DotProduct(currentDirection, startingDirection) < 0.0f) {
						continue;
					}
				}*/

				FNavPathNode node;
				node.location = pathPoints[i].Location;
				currentPath.Add(node);

			}
			if (currentPath.Num() == 1) {
				currentPath.Empty();
			}
		}
	}
}

FNavAgentProperties UMindComponent::getNavProperties() {
	FNavAgentProperties navAgentProperties;

	ACreature* creature = Cast<ACreature>(GetOwner());
	if (!creature) {
		return navAgentProperties;
	}

	navAgentProperties.AgentHeight = creature->getCapsuleHalfHeight() * 2.0f;
	navAgentProperties.AgentRadius = creature->getCapsuleRadius();
	navAgentProperties.bCanWalk = true;
	navAgentProperties.bCanFly = false;
	return navAgentProperties;
}

void UMindComponent::tryJump(ACreature* creature) {
	if (creature->canStartJump() && jumpCooldown <= 0.0f) {
		creature->startJump();
		jumpCooldown = 0.5f;
	}
}

bool UMindComponent::moveTowards(float deltaTime, ACreature* creature, FVector location, float range) {
	FVector creatureLocation = creature->GetActorLocation();
	findPath(creature, location, deltaTime);

	if (currentPath.Num() > 0) {
		location = currentPath[0].location;
		location.Z += creature->getCapsuleHalfHeight();

		if (debugPathing) {
			for (int i = 0; i < currentPath.Num(); i++) {
				FColor color = FColor::Orange;
				if (currentPath[i].link) {
					if (currentPath[i].link->pathType == ENavPathNodeType::JUMP) {
						color = FColor::Green;
					}
				}

				if (i > 0) {
					UDebug::drawLine(GetWorld(), currentPath[i - 1].location, currentPath[i].location, color, 5.0f, 0.05f);
				}
				else {
					UDebug::drawLine(GetWorld(), GetOwner()->GetActorLocation(), currentPath[i].location, color, 5.0f, 0.05f);
				}
				UDebug::drawPoint(GetWorld(), currentPath[i].location, 15.0f, color, 0.05f);
			}
		}

	}
	else {
		if (debugPathing) {
			UDebug::drawLine(GetWorld(), GetOwner()->GetActorLocation(), location, FColor::Red, 5.0f, 0.05f);
			UDebug::drawPoint(GetWorld(), location, 15.0f, FColor::Red, 0.05f);
		}
	}

	float distXY = FVector::DistXY(creatureLocation, location) - (creature->getCapsuleRadius() + range);
	if (distXY <= 0.0f && FMath::Abs(creatureLocation.Z - location.Z) < creature->getCapsuleHalfHeight() + range) {
		if (currentPath.Num() > 0) {
			currentPath.RemoveAt(0);
			return false;
		}
		else {
			return true;
		}
	}

	float targetYaw = UMathTools::getYaw(creatureLocation, location);
	float yawDiff = UMathTools::getAngleDiff(targetYaw, creature->bodyYaw);
	creature->inputMove = FVector2D(0.0f, 1.0f).GetRotated(yawDiff);

	if (distXY <= 125.0f && creatureLocation.Z < location.Z - 25.0f) {
		tryJump(creature);
	}

	return false;
}

void UMindComponent::registerSeeActor(AActor* actor) {
	if (perception) {
		/*FAIStimulus stimulus;

		sight->GetSenseImplementation()
		FAIStimulus stimulus(UAISense::GetSenseID<UAISense_Sight>(),1.0f, actor->GetActorLocation(), GetOwner()->GetActorLocation()), FAIStimulus::SensingSucceeded);
		(
			const UAISense & Sense,
			float StimulusStrength,
			const FVector & InStimulusLocation,
			const FVector & InReceiverLocation,
			FResult Result,
			FName InStimulusTag
		)

		perception->RegisterStimulus(actor);*/
	}
}


void UMindComponent::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type", FString("Mind"));
}

void UMindComponent::readXML(FXmlNode* rootNode) {

}


UMindComponent* UMindComponent::get(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	return Cast< UMindComponent >(actor->GetComponentByClass(UMindComponent::StaticClass()));
}

UMindComponent* UMindComponent::add(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	UMindComponent* mind = get(actor);
	if (mind) {
		return mind;
	}


	UClass* mindClass = nullptr;

	UTeam* team = UTeam::get(actor);
	if (team && team->defaultMindComponentClass != nullptr) {
		mindClass = team->defaultMindComponentClass;
	}
	else {
		if (actor->IsA(ACreature::StaticClass())) {
			ACreature* creature = Cast< ACreature >(actor);
			if (creature->spec) {
				mindClass = creature->spec->mindComponentClass;
			}
		}
		if (mindClass == nullptr) {
			mindClass = UMindComponent::StaticClass();
		}
	}

	mind = NewObject< UMindComponent >(actor, mindClass);
	mind->RegisterComponent();
	return mind;
}

void UMindComponent::remove(AActor* actor) {
	if (actor == nullptr) {
		return;
	}
	UMindComponent* mind = get(actor);
	if (!mind) {
		return;
	}
	mind->DestroyComponent();
}