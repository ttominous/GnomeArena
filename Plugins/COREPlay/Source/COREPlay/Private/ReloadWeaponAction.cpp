#include "ReloadWeaponAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "ProjectileWeaponItem.h"
#include "ProjectileWeaponItemSpec.h"
#include "Projectile.h"
#include "MathTools.h"
#include "Debug.h"

bool UReloadWeaponAction::canDoAction(AActor* actor) {

	UEquipmentComponent* equipment = UEquipmentComponent::get(actor);
	if (!equipment) {
		return false;
	}
	UInventoryComponent* inventory = UInventoryComponent::get(actor);
	if (!inventory) {
		return true;
	}
	UProjectileWeaponItem* item = Cast< UProjectileWeaponItem >(inventory->getEquippedItem("PrimaryWeapon"));
	if (!item) {
		return false;
	}

	UProjectileWeaponItemSpec* itemSpec = Cast< UProjectileWeaponItemSpec >(item->getItemSpec());
	if (!itemSpec) {
		return false;
	}

	if (item->clipCount >= itemSpec->clipSize) {
		return false;
	}
	return inventory->countItem(item->ammoItemKey) > 0;
}


void UReloadWeaponAction::processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

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

	if (playAnimInstance != nullptr && playAnimInstance->pingLog.Contains("FirstPing")) {
		return;
	}
	if (playAnimInstance) {
		playAnimInstance->pingLog.Add("FirstPing");
	}

	if (itemSpec->effects.Contains("Reload") && itemSpec->effects["Reload"] != nullptr) {
		itemSpec->effects["Reload"]->execute(creature, FVector::ZeroVector, FRotator(90, 0, 0), FVector(0.1f), "Casing", creature->getWeaponComponent());
	}

	if (!creature->GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		UInventoryComponent* inventory = UInventoryComponent::get(component->GetOwner());
		if (inventory) {
			UProjectileWeaponItem* item = Cast< UProjectileWeaponItem >(inventory->getEquippedItem("PrimaryWeapon"));
			if (item) {
				int amountToLoad = FMath::Min(itemSpec->clipSize - item->clipCount, itemSpec->reloadAmount);
				amountToLoad = FMath::Min(inventory->countItem(item->ammoItemKey), amountToLoad);

				inventory->removeItem(item->ammoItemKey, amountToLoad);
				item->clipCount += amountToLoad;
			}
		}
	}
}


void UReloadWeaponAction::processAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	Super::processAnimationFinished(instance, component);

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

	UInventoryComponent* inventory = UInventoryComponent::get(creature);
	if (canDoAction(component->GetOwner())) {
		component->startBaseAction("ReloadWeapon");
		return;
	}
	if (itemSpec->pumpAfterReloading) {
		component->startBaseAction("PumpWeapon");
	}
}