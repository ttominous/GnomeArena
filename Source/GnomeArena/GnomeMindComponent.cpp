#include "GnomeMindComponent.h"
#include "DualAxisCreature.h"
#include "DropItem.h"
#include "EquipmentComponent.h"
#include "StatsComponent.h"
#include "WeaponItemSpec.h"
#include "ComplexGameState.h"
#include "ActionComponent.h"
#include "PointOfInterest.h"
#include "MathTools.h"
#include "Debug.h"


void UGnomeMindComponent::update(ACreature* creature, float deltaTime) {
	Super::update(creature, deltaTime);

	if (!creature->isBot) {
		return;
	}

	ADualAxisCreature* dualAxisCreature = Cast< ADualAxisCreature >(creature);
	if (!dualAxisCreature) {
		return;
	};


	updateTarget(dualAxisCreature,deltaTime);

	if (target.isPopulated() && target.type != ETargetClass::LOCATION) {
		strafeTimer -= deltaTime;
		if (strafeTimer <= 0.0f) {
			strafeType = (EStrafeType)FMath::RandRange(0, 5);
			if (strafeType == EStrafeType::BACK) {
				strafeTimer = FMath::RandRange(0.1f, 0.5f);
			}
			else {
				strafeTimer = FMath::RandRange(0.1f, 2.0f);
			}
		}
	}
	else {
		if (strafeTimer > 0.0f) {
			strafeTimer = FMath::Min(strafeTimer, 1.0f);
			strafeTimer -= deltaTime;
			if (strafeTimer <= 0.0f) {
				strafeType = EStrafeType::NONE;
			}
		}
	}

	if (primaryAttackCooldown > 0.0f) {
		primaryAttackCooldown -= deltaTime;
	}
	if (primaryBurstTime > 0.0f) {
		primaryBurstTime -= deltaTime;
	}
	if (secondaryAttackCooldown > 0.0f) {
		secondaryAttackCooldown -= deltaTime;
	}


	bool primaryAttack = false;
	bool secondaryAttack = false;
	bool sprinting = false;
	bool aiming = false;
	creature->inputMove = FVector2D::ZeroVector;

	if (target.isPopulated()) {

		FVector myLocation = creature->GetActorLocation();
		FVector feetLocation = myLocation;
		feetLocation.Z -= creature->getCapsuleHalfHeight();
		FVector targetLocation;
		float targetDist;
		bool hostile = false;
		bool turnTowards = false;

		float primaryAttackRange = 75.0f;
		float secondaryAttackRange = 100.0f;
		UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
		if (equipment) {
			UWeaponItemSpec* weaponSpec = equipment->getEquippedWeapon("PrimaryWeapon", true, true);
			if (weaponSpec) {
				primaryAttackRange = weaponSpec->aiMeleeRange;
			}
		}

		if (target.type == ETargetClass::CREATURE) {
			ACreature* targetCreature = target.getCreature(GetWorld());
			if (targetCreature == nullptr) {
				target = FTargetDetail();
				return;
			}
			targetLocation = targetCreature->GetActorLocation();
			hostile = true;
			turnTowards = true;
		}
		else if (target.type == ETargetClass::DROP_ITEM) {
			ADropItem* dropItem = target.getDropItem(GetWorld());
			if (dropItem == nullptr) {
				target = FTargetDetail();
				return;
			}
			targetLocation = dropItem->GetActorLocation();
		}
		else {
			targetLocation = target.location;
		}
		targetDist = FVector::Dist(myLocation, targetLocation);
		dualAxisCreature->aimAtPoint = targetLocation;

		FRotator facingRotation = dualAxisCreature->controlRotation;
		facingRotation.Pitch = 0.0f;

		bool overlapping = false;
		bool followingPath = currentPath.Num() > 0;
		if (currentPath.Num() > 0 && currentPath[0].link != nullptr) {
			if (currentPath[0].link->pathType == ENavPathNodeType::JUMP) {
				tryJump(creature);
				overlapping = moveTowards(deltaTime, creature, targetLocation, 25.0f);
			}
			else if (currentPath[0].link->pathType == ENavPathNodeType::WALK) {
				overlapping = moveTowards(deltaTime, creature, targetLocation, 25.0f);
			}
		}
		else {
			overlapping = moveTowards(deltaTime, creature, targetLocation, 25.0f);

			if (!followingPath) {
				float heightDist = targetLocation.Z - myLocation.Z;
				if (heightDist >= 25.0f) {
					tryJump(creature);
				}
			}
		}
		followingPath = currentPath.Num() > 0;

		if (!turnTowards) {
			if (followingPath) {
				facingRotation.Yaw = UMathTools::getYaw(myLocation, currentPath[0].location) + 90.0f;
			}
			else {
				if (targetDist > 75.0f) {
					facingRotation.Yaw = UMathTools::getYaw(myLocation, targetLocation) + 90.0f;
				}
			}
		}
		else {
			if (followingPath) {
				facingRotation.Yaw = UMathTools::getYaw(myLocation, currentPath[0].location) + 90.0f;
			}
			else {
				facingRotation = UMathTools::getYawAndPitch(myLocation, targetLocation);
			}
		}
		dualAxisCreature->controlRotation = UMathTools::lerpRotatorByPerc(dualAxisCreature->controlRotation, facingRotation, deltaTime * 10.0f);

		if (hostile) {

			float facingYaw = UMathTools::getYaw(myLocation, targetLocation);
			bool facingTarget = UMathTools::getAngleDiff(facingYaw, creature->bodyYaw, true) < 10.0f;

			if (facingTarget) {
				if (targetDist <= secondaryAttackRange) {
					if (secondaryAttackCooldown <= 0.0f) {
						if (FMath::RandRange(0.0f, 1.0f) <= 0.6f) {
							secondaryAttack = true;
							dualAxisCreature->secondaryAttack = false;
						}
						secondaryAttackCooldown = FMath::RandRange(0.25f, 1.0f);
					}
				}
				if (!secondaryAttack) {

					if (targetDist <= primaryAttackRange) {

						if (primaryAttackCooldown <= 0.0f) {
							primaryAttackCooldown = FMath::RandRange(0.1f, 1.5f);
							primaryBurstTime = FMath::RandRange(0.1f, 2.0f);
						}

						if (primaryBurstTime > 0.0f) {
							primaryAttack = true;
							dualAxisCreature->primaryAttack = false;
						}
					}
					else if (targetDist >= 200.0f) {
						sprinting = true;
					}
				}
			}
		}
		else if(targetDist <= 100.0f){
			target = FTargetDetail();
			targetRefreshTimer = 0.0f;
		}
	}
	else {
		FRotator relaxedRotation = dualAxisCreature->controlRotation;
		relaxedRotation.Pitch = 0.0f;

		dualAxisCreature->controlRotation = UMathTools::lerpRotatorByPerc(dualAxisCreature->controlRotation, relaxedRotation, deltaTime * 10.0f);
	}

	if (strafeType == EStrafeType::BACK) {
		creature->inputMove.Y = -0.5f;
	}
	else if (strafeType == EStrafeType::FORWARD) {
		if (!primaryAttack && !secondaryAttack) {
			creature->inputMove.Y = 1.0f;
			sprinting = true;
		}
		else {
			creature->inputMove.Y = FMath::Max(creature->inputMove.Y, 0.5f);
		}
	}
	else if (strafeType == EStrafeType::RIGHT) {
		creature->inputMove.X = 0.5f;
	}
	else if (strafeType == EStrafeType::LEFT) {
		creature->inputMove.X = -0.5f;
	}
	else if (strafeType == EStrafeType::JUMP) {
		strafeType = EStrafeType::NONE;
		if (FMath::RandRange(0.0f, 1.0f) <= 0.35f) {
			tryJump(creature);
		}
	}

	if (dualAxisCreature->primaryAttack != primaryAttack) {
		dualAxisCreature->primaryAttack = primaryAttack;
		if (primaryAttack) {
			dualAxisCreature->getActionComponent()->processInputPress("PrimaryAttack");
		}
		else {
			dualAxisCreature->getActionComponent()->processInputRelease("PrimaryAttack");
		}
	}
	if (dualAxisCreature->secondaryAttack != secondaryAttack) {
		dualAxisCreature->secondaryAttack = secondaryAttack;
		if (secondaryAttack) {
			dualAxisCreature->getActionComponent()->processInputPress("SecondaryAttack");
		}
		else {
			dualAxisCreature->getActionComponent()->processInputRelease("SecondaryAttack");
		}
	}
	dualAxisCreature->sprinting = sprinting;
	dualAxisCreature->aiming = aiming;
}

void UGnomeMindComponent::updateTarget(ADualAxisCreature* creature, float deltaTime) {

	targetRefreshTimer -= deltaTime;

	if (target.isPopulated()) {

		if (target.type == ETargetClass::CREATURE) {
			ACreature* targetCreature = target.getCreature(GetWorld());
			if (targetCreature == nullptr || targetCreature->dead) {
				target = FTargetDetail();
				targetRefreshTimer = 0.0f;
				return;
			}
		}
		else if (target.type == ETargetClass::DROP_ITEM) {
			ADropItem* targetDropItem = target.getDropItem(GetWorld());
			if (targetDropItem == nullptr) {
				target = FTargetDetail();
				targetRefreshTimer = 0.0f;
				return;
			}
		}

		if (targetRefreshTimer <= 0.0f) {
			target = (target.type == ETargetClass::LOCATION || target.type == ETargetClass::DROP_ITEM) ? target : FTargetDetail();
			targetRefreshTimer = target.type == ETargetClass::LOCATION ? 1.0f : 2.0f;
		}
		else {
			return;
		}

	}
	else {
		if (targetRefreshTimer <= 0.0f) {
			targetRefreshTimer = 1.0f;
		}
		else {
			return;
		}
	}


	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}
	UTeam* myTeam = gameState->getTeam(GetOwner());
	FVector myLocation = GetOwner()->GetActorLocation();

	UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
	UWeaponItemSpec* weaponSpec = nullptr;
	if (equipment) {
		weaponSpec = equipment->getEquippedWeapon("PrimaryWeapon", true, true);
	}

	UStatsComponent* stats = UStatsComponent::get(creature);
	float healthPerc = stats != nullptr ? stats->getStatPerc("HEALTH") : 1.0f;


	ACreature* bestCreatureTarget = nullptr;
	float bestCreatureTargetDist = 0.0f;

	ADropItem* bestDropItemTarget = nullptr;
	float bestDropItemTargetDist = 0.0f;

	EDropItemType preferredDropItemType = EDropItemType::UNKNOWN;
	if (weaponSpec == nullptr) {
		preferredDropItemType = EDropItemType::WEAPON;
	}
	if (healthPerc < 0.65f) {
		preferredDropItemType = EDropItemType::HEALTH;
	}


	TArray< AActor* > knownActors;
	if (perception) {
		perception->GetKnownPerceivedActors(nullptr, knownActors);
	}

	/*
		Find the best Creature/Living Target
	*/
	for (UTeam* team : gameState->teams) {
		if (team == myTeam) {
			continue;
		}
		for (UPlayerRecord* player : team->players) {
			ACreature* targetCreature = Cast< ACreature >(player->pawn);
			if (targetCreature == nullptr || targetCreature->dead) {
				continue;
			}

			if (perception && !knownActors.Contains(targetCreature)) {
				continue;
			}

			float dist = FVector::Dist(myLocation, targetCreature->GetActorLocation());
			if (bestCreatureTarget == nullptr || bestCreatureTargetDist > dist) {
				bestCreatureTarget = targetCreature;
				bestCreatureTargetDist = dist;
			}
		}
	}

	/*
		Find the best Drop Item to try to pick up
	*/
	for (AActor* actor : knownActors) {
		if (actor == nullptr || !actor->IsA(ADropItem::StaticClass())) {
			continue;
		}

		ADropItem* dropItem = Cast<ADropItem>(actor);
		if (dropItem->type != preferredDropItemType) {
			continue;
		}

		float dist = FVector::Dist(myLocation, dropItem->GetActorLocation());
		if (bestDropItemTarget == nullptr || bestDropItemTargetDist > dist) {
			bestDropItemTarget = dropItem;
			bestDropItemTargetDist = dist;
		}
	}

	if (bestDropItemTarget != nullptr && bestCreatureTarget != nullptr) {
		if (bestCreatureTargetDist >= bestDropItemTargetDist && bestCreatureTargetDist >= 300.0f) {
			bestCreatureTarget = nullptr;
		}
	}


	if (bestCreatureTarget) {
		target = FTargetDetail(bestCreatureTarget);
		return;
	}
	if (bestDropItemTarget) {
		target = FTargetDetail(bestDropItemTarget);
		return;
	}

	if (target.isEmpty() && APointOfInterest::instances.Num() > 0) {
		APointOfInterest* pointOfInterest = APointOfInterest::instances[FMath::RandRange(0,APointOfInterest::instances.Num()-1)];
		if (pointOfInterest) {
			target = FTargetDetail(pointOfInterest->GetActorLocation());
		}
	}

}