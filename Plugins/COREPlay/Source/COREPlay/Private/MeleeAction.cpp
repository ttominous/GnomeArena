#include "MeleeAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "DualAxisAnimInstance.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "EquipmentComponent.h"
#include "BodyComponent.h"
#include "WeaponItemSpec.h"
#include "CollisionTools.h"
#include "Debug.h"


#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"



void UMeleeAction::processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

	UMeleeActionInstance* meleeActionInstance = Cast< UMeleeActionInstance >(instance);
	if (!meleeActionInstance) {
		return;
	}

	if (!component->GetWorld()->IsNetMode(ENetMode::NM_Client)) {


		ACreature* creature = Cast< ACreature >(component->GetOwner());
		if (creature) {

			if (!meleeActionInstance->damageApplied) {
				meleeActionInstance->damageApplied = true;

				FString boneName = payload.getString("BoneName");

				UWeaponItemSpec* weaponSpec = nullptr;
				USceneComponent* weaponComponent = nullptr;
				UEquipmentComponent* equipment = UEquipmentComponent::get(component->GetOwner());
				if (equipment) {
					weaponSpec = equipment->getEquippedWeapon(boneName, false, true);
					weaponComponent = equipment->getComponent(boneName);
				}

				FVector boxExtent = payload.getVector("BoxSize", FVector(25.0f));
				FVector boxOffset = payload.getVector("BoxOffset", FVector::ZeroVector);
				FRotator boxRotation = payload.getRotator("BoxRotation", FRotator::ZeroRotator);

				if (weaponSpec && weaponSpec->overrideMeleeDamageBox) {
					boxExtent = weaponSpec->meleeDamageBoxExtent;
					boxOffset = weaponSpec->meleeDamageBoxOffset;
					boxRotation = weaponSpec->meleeDamageBoxRotation;
				}

				FRotator actorRotation = FRotator(0.0f, creature->bodyYaw, 0.0f);
				if (useControlPitch) {
					actorRotation.Roll = -creature->controlRotation.Pitch;
				}
				boxRotation += actorRotation;
				boxOffset = actorRotation.RotateVector(boxOffset);



				FVector bonePosition = payload.getVector("HittingBone", creature->GetActorLocation());
				if (weaponSpec && weaponSpec->overrideMeleeDamageBox && weaponComponent) {
					if (weaponComponent->IsA(UStaticMeshComponent::StaticClass())) {
						UStaticMeshComponent* meshComponent = Cast< UStaticMeshComponent >(weaponComponent);
						bonePosition = meshComponent->GetSocketTransform(FName(*weaponSpec->meleeDamageBoxWeaponSocket), ERelativeTransformSpace::RTS_World).GetLocation();
					}
					else if (weaponComponent->IsA(USkeletalMeshComponent::StaticClass())) {
						USkeletalMeshComponent* meshComponent = Cast< USkeletalMeshComponent >(weaponComponent);
						bonePosition = meshComponent->GetBoneLocation(FName(*weaponSpec->meleeDamageBoxWeaponSocket), EBoneSpaces::Type::WorldSpace);
					}
				}
				
				
				FVector boxPosition = bonePosition + boxOffset;
				if (debug) {
					UDebug::drawBox(component->GetWorld(), boxPosition, boxRotation.Quaternion(), boxExtent, FColor::Red, 2.0f, 5.0f);
				}

				TMap< AActor*, FOverlapSet > overlappedActors = UCollisionTools::getBoxOverlapsByActor(component->GetWorld(), boxPosition, boxRotation, boxExtent, ECollisionChannel::ECC_WorldStatic);

				for (auto& overlappedActor : overlappedActors) {
					AActor* actor = overlappedActor.Key;
					if (overlappedActor.Value.overlaps.Num() == 0 || actor == nullptr || actor == component->GetOwner() || !actor->IsA(ACreature::StaticClass())) {
						continue;
					}

					ACreature* victim = Cast<ACreature>(actor);
					FVector victimLocation = victim->GetActorLocation();


					FString bestBoneName;
					float bestOverlapDist = 0.0f;
					FVector bestOverlapPosition;

					for (FOverlapResult& overlap : overlappedActor.Value.overlaps) {

						if (overlap.GetComponent() != victim->GetMesh()) {
							continue;
						}

						FName hitBoneName = victim->GetMesh()->GetBoneName(overlap.ItemIndex);
						FVector hitLocation = victim->GetMesh()->GetSocketLocation(hitBoneName);
						float dist = FVector::Dist(bonePosition, hitLocation);
						if (bestBoneName.Len() == 0 || bestOverlapDist > dist) {
							bestOverlapDist = dist;
							bestOverlapPosition = hitLocation;
							bestBoneName = hitBoneName.ToString();
						}

					}

					if (bestBoneName.Len() == 0) {
						continue;
					}

					FDamageDetails damage = baseDamage;
					if (weaponSpec) {
						damage = damage.add(weaponSpec->baseMeleeDamage);
					}

					if (damage.iconKey.Len() == 0) {
						damage.iconKey = iconKey;
					}
					damage.attacker = FTargetDetail(component->GetOwner());
					damage.attacker.location = component->GetOwner()->GetActorLocation();
					damage.victim = FTargetDetail(victim);
					damage.victim.location = (bonePosition + bestOverlapPosition) / 2.0f;
					damage.victim.setComponentName(victim->GetMesh()->GetName());
					damage.victim.setHitBoneName(bestBoneName);

					victim->processServerDamage(damage);

					if (debug) {
						UDebug::print("Hit " + actor->GetName() + " - " + bestBoneName);
					}
				}



			}
			else if (meleeActionInstance->comboRequested && nextComboActionKey.Len() > 0) {
				creature->sendPayload(FPayload::startAction(nextComboActionKey));
				return;
			}
		}

	}
}

FString UMeleeAction::playAnimation(FPlayAnimationData& animationData, ACreature* creature) {

	ADualAxisCreature* dualAxisCreature = Cast< ADualAxisCreature >(creature);

	FString sequenceKey;
	if (!animationData.slot.Equals("DefaultSlot") && creature->getMovePerc() < 0.05f && (dualAxisCreature == nullptr || !dualAxisCreature->crouched)) {
		FPlayAnimationData finalAnimationData = animationData;
		finalAnimationData.slotState = ESlotAnimState::FULL_BODY;
		finalAnimationData.changeSlotState = true;
		sequenceKey = Super::playAnimation(finalAnimationData, creature);
	}
	else {
		sequenceKey = Super::playAnimation(animationData, creature);
	}

	if (sequenceKey.Len() > 0 && creature) {
		UDualAxisAnimInstance* animInstance = Cast< UDualAxisAnimInstance >(creature->getAnimInstance());
		if (animInstance) {
			animInstance->aimRotationEnabled = animationData.aimRotationEnabled;
			animInstance->torsoSlotState = animationData.slotState;
		}
	}
	return sequenceKey;
}

void UMeleeAction::processInputPress(FString inputKey, UActionInstance* instance, UActionComponent* component) {
	if (inputKey.Equals(comboInputKey)) {
		UMeleeActionInstance* meleeActionInstance = Cast< UMeleeActionInstance >(instance);
		if (!meleeActionInstance) {
			return;
		}
		meleeActionInstance->comboRequested = true;
	}
}

bool UMeleeAction::isRelevantInput(FString inputKey) {
	return inputKey.Equals(comboInputKey);
}


UClass* UMeleeAction::getInstanceClass(UActionComponent* component) {
	return UMeleeActionInstance::StaticClass();
}