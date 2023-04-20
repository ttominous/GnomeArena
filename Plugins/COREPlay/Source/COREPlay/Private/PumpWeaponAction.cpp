#pragma once
#include "PumpWeaponAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "CreatureAnimInstance.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "ProjectileWeaponItemSpec.h"
#include "Debug.h"


void UPumpWeaponAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
	if (!equipment) {
		return;
	}
	UProjectileWeaponItemSpec* itemSpec = Cast< UProjectileWeaponItemSpec >(equipment->getEquippedItemSpec("PrimaryWeapon"));
	if (!itemSpec) {
		return;
	}

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (creature->isAiming() && itemSpec->animationDatas.Contains("PumpAiming")) {
		playAnimInstance->animSequence = playAnimation(itemSpec->animationDatas["PumpAiming"], creature);
		if (animInstance) {
			animInstance->processPlayAnimStart(itemSpec->animationDatas["PumpAiming"]);
		}
	}
	else if (itemSpec->animationDatas.Contains("Pump")) {
		playAnimInstance->animSequence = playAnimation(itemSpec->animationDatas["Pump"], creature);
		if (animInstance) {
			animInstance->processPlayAnimStart(itemSpec->animationDatas["Pump"]);
		}
	}
}

void UPumpWeaponAction::processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
	if (!equipment) {
		return;
	}
	UProjectileWeaponItemSpec* itemSpec = Cast< UProjectileWeaponItemSpec >(equipment->getEquippedItemSpec("PrimaryWeapon"));
	if (!itemSpec) {
		return;
	}

	FString effectKey = payload.getString("EffectKey");
	if (playAnimInstance == nullptr || !playAnimInstance->pingLog.Contains(effectKey)) {
		if (itemSpec->effects.Contains(effectKey) && itemSpec->effects[effectKey] != nullptr) {
			itemSpec->effects[effectKey]->execute(creature, FVector::ZeroVector, FRotator(90, 0, 0), FVector(0.1f), "Casing", creature->getWeaponComponent());
		}

		if (playAnimInstance) {
			playAnimInstance->pingLog.Add(effectKey);
		}
	}

}
