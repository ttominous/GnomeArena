#include "FollowerMindComponent.h"
#include "DualAxisCreature.h"
#include "ComplexGameState.h"
#include "ActionComponent.h"
#include "PointOfInterest.h"
#include "MathTools.h"
#include "Debug.h"


void UFollowerMindComponent::update(ACreature* creature, float deltaTime) {
	Super::update(creature, deltaTime);

	if (!creature->isBot) {
		return;
	}

	ADualAxisCreature* dualAxisCreature = Cast< ADualAxisCreature >(creature);
	if (!dualAxisCreature) {
		return;
	};


	FVector myLocation = creature->GetActorLocation();
	bool sprinting = true;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController && playerController->GetPawn()) {

			FVector targetLocation = playerController->GetPawn()->GetActorLocation();

			if (currentPath.Num() > 0 && currentPath[0].link != nullptr) {
				if (currentPath[0].link->pathType == ENavPathNodeType::JUMP) {
					tryJump(creature);
					 moveTowards(deltaTime, creature, targetLocation, 25.0f);
				}
				else if (currentPath[0].link->pathType == ENavPathNodeType::WALK) {
					moveTowards(deltaTime, creature, targetLocation, 25.0f);
				}
			}
			else {
				moveTowards(deltaTime, creature, targetLocation, 25.0f);

				if (currentPath.Num() == 0) {
					float heightDist = targetLocation.Z - myLocation.Z;
					if (heightDist >= 25.0f) {
						tryJump(creature);
					}
				}
			}


			FRotator facingRotation = UMathTools::getYawAndPitch(myLocation, targetLocation);
			dualAxisCreature->controlRotation = UMathTools::lerpRotatorByPerc(dualAxisCreature->controlRotation, facingRotation, deltaTime * 10.0f);

			float targetDist = FVector::Dist(myLocation, targetLocation);
			if (targetDist >= 400.0f) {
				sprinting = true;
			}

			return;
		}
	}

	dualAxisCreature->sprinting = sprinting;

}