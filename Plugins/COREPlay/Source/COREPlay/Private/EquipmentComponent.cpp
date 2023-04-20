#include "EquipmentComponent.h"
#include "BodyComponent.h"
#include "ActionComponent.h"
#include "ProjectileWeaponItemSpec.h"
#include "ResourceCache.h"
#include "WearableItemSpec.h"
#include "CreatureSpec.h"
#include "DualAxisAnimInstance.h"
#include "StringTools.h"
#include "Debug.h"



UEquipmentComponent::UEquipmentComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


UEquipmentComponent* UEquipmentComponent::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}
	return Cast< UEquipmentComponent >(actor->GetComponentByClass(UEquipmentComponent::StaticClass()));
}

bool UEquipmentComponent::hasSlot(FString slotKey) {
	return getSlotIndex(slotKey) > -1;
}

int UEquipmentComponent::getSlotIndex(FString slotKey) {
	for (int i = 0; i < slots.Num(); i++) {
		if (slots[i].key.Equals(slotKey)) {
			return i;
		}
	}
	return -1;
}

bool UEquipmentComponent::setEquippedItem(FString slotKey, FString itemKey, int itemIndex) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return false;
	}

	FString previousItemKey = slots[slotIndex].itemKey;
	slots[slotIndex].itemKey = itemKey;
	slots[slotIndex].itemIndex = itemIndex;

	postProcessUnEquipItem(slotKey, previousItemKey);
	postProcessEquipItem(slotKey, itemKey);

	return true;
}

int UEquipmentComponent::getItemIndex(FString slotKey) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return -1;
	}
	return slots[slotIndex].itemIndex;
}

FString UEquipmentComponent::getItemKey(FString slotKey) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return "";
	}
	return slots[slotIndex].itemKey;
}

bool UEquipmentComponent::unequipItem(FString slotKey) {
	return setEquippedItem(slotKey, "", -1);
}

bool UEquipmentComponent::unequipAllItems() {
	for (int i = 0; i < slots.Num(); i++) {
		setEquippedItem(slots[i].key, "", -1);
	}
	return true;
}

bool UEquipmentComponent::hasEquippedItem(FString slotKey) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return false;
	}
	return slots[slotIndex].itemKey.Len() > 0;
}

UItemSpec* UEquipmentComponent::getEquippedItemSpec(FString slotKey, bool considerTemporary) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return false;
	}
	return UResourceCache::getItemSpec(considerTemporary ? UStringTools::defaultString(slots[slotIndex].temporaryItemKey, slots[slotIndex].itemKey) : slots[slotIndex].itemKey);
}

UWeaponItemSpec* UEquipmentComponent::getEquippedWeapon(FString preferredSlotKey, bool allowAnyWeapon, bool considerTemporary) {

	if (!hasSlot(preferredSlotKey)) {
		if (UStringTools::containsIgnoreCase(preferredSlotKey, "Hand")) {
			if (UStringTools::containsIgnoreCase(preferredSlotKey, "Left")) {
				preferredSlotKey = "LeftWeapon";
			}
			else {
				preferredSlotKey = "RightWeapon";
			}
		}
	}

	UWeaponItemSpec* targetedWeaponSpec = Cast< UWeaponItemSpec >(getEquippedItemSpec(preferredSlotKey, considerTemporary));
	if (targetedWeaponSpec) {
		return targetedWeaponSpec;
	}

	if (allowAnyWeapon) {
		for (int i = 0; i < slots.Num(); i++) {
			UWeaponItemSpec* weaponSpec = Cast< UWeaponItemSpec >(UResourceCache::getItemSpec(slots[i].itemKey));
			if (weaponSpec) {
				return weaponSpec;
			}
		}
	}
	return nullptr;
}


bool UEquipmentComponent::hasTemporaryItem(FString slotKey) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return false;
	}
	return slots[slotIndex].temporaryItemKey.Len() > 0;
}

bool UEquipmentComponent::setTemporaryItem(FString slotKey, FString itemKey) {
	int slotIndex = getSlotIndex(slotKey);
	if (slotIndex == -1) {
		return false;
	}
	slots[slotIndex].temporaryItemKey = itemKey;
	markDirty();
	return true;
}

bool UEquipmentComponent::clearTemporaryItem(FString slotKey) {
	return setTemporaryItem(slotKey, "");
}

bool UEquipmentComponent::clearAllTemporaryItems(FString slotKey) {
	for (int i = 0; i < slots.Num(); i++) {
		clearTemporaryItem(slots[i].key);
	}
	return true;
}

void UEquipmentComponent::markDirty() {
	UBodyComponent* body = UBodyComponent::get(GetOwner());
	if (body) {
		body->dirty = true;
	}
}

void UEquipmentComponent::postProcessEquipItem(FString slotKey, FString itemKey) {
	UItemSpec* itemSpec = UResourceCache::getItemSpec(itemKey);
	if (!itemSpec) {
		return;
	}

	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		if (itemSpec->IsA(UProjectileWeaponItemSpec::StaticClass())) {
			UProjectileWeaponItemSpec* weaponSpec = Cast< UProjectileWeaponItemSpec >(itemSpec);
			if (weaponSpec->ammoItemKeys.Num() > 0) {
				UInventoryComponent* inventory = UInventoryComponent::get(GetOwner());
				if (inventory) {
					UProjectileWeaponItem* item = Cast< UProjectileWeaponItem>(inventory->getEquippedItem(slotKey));
					if (item && item->ammoItemKey.Len() == 0) {
						for (FString ammoItemKey : weaponSpec->ammoItemKeys) {
							if (inventory->hasItem(ammoItemKey)) {
								item->ammoItemKey = ammoItemKey;
								break;
							}
						}
					}
				}
			}

		}
	}

	markDirty();
}

void UEquipmentComponent::postProcessUnEquipItem(FString slotKey, FString itemKey) {

	UItemSpec* itemSpec = UResourceCache::getItemSpec(itemKey);
	if (!itemSpec) {
		return;
	}

	if (itemSpec->IsA(UWeaponItemSpec::StaticClass())) {
		initAnimInstance();
		initActionSets();
	}

	markDirty();
}

void UEquipmentComponent::refresh() {
	for (int i = 0; i < slots.Num(); i++) {
		setEquippedItem(slots[i].key, slots[i].itemKey, slots[i].itemIndex);
	}
	initAnimInstance();
	initActionSets();
}

void UEquipmentComponent::initAnimInstance() {
	ACreature* creature = Cast< ACreature >(GetOwner());
	if (!creature) {
		return;
	}

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (!animInstance) {
		return;
	}
	UDualAxisAnimInstance* dualAxisAnimInstance = Cast< UDualAxisAnimInstance >(animInstance);


	UCreatureSpec* creatureSpec = creature->spec;
	if (creatureSpec && dualAxisAnimInstance) {
		dualAxisAnimInstance->defaultAimBoneAmounts = creatureSpec->defaultAimBoneAmounts;
		dualAxisAnimInstance->defaultAimRotationEnabled = creatureSpec->defaultAimRotationEnabled;
		dualAxisAnimInstance->defaultTorsoSlotState = creatureSpec->defaultTorsoSlotState;
		dualAxisAnimInstance->defaultWeaponHandSnapEnabled = creatureSpec->defaultWeaponHandSnapEnabled;
		dualAxisAnimInstance->defaultIdleArmsBlendAmounts = creatureSpec->defaultIdleArmsBlendAmounts;
	}


	UAnimationSet* weaponAnimationSet = nullptr;
	for (int i = 0; i < slots.Num(); i++) {

		UItemSpec* itemSpec = UResourceCache::getItemSpec(slots[i].itemKey);
		if (itemSpec) {
			if (itemSpec->IsA(UWeaponItemSpec::StaticClass())) {
				UWeaponItemSpec* weaponSpec = Cast< UWeaponItemSpec >(itemSpec);
				weaponAnimationSet = weaponSpec->animationSet;

				if (dualAxisAnimInstance) {
					dualAxisAnimInstance->defaultAimBoneAmounts = weaponSpec->defaultAimBoneAmounts;
					dualAxisAnimInstance->defaultAimRotationEnabled = weaponSpec->defaultAimRotationEnabled;
					dualAxisAnimInstance->defaultTorsoSlotState = weaponSpec->defaultTorsoSlotState;
					dualAxisAnimInstance->defaultWeaponHandSnapEnabled = weaponSpec->defaultWeaponHandSnapEnabled;
					dualAxisAnimInstance->defaultIdleArmsBlendAmounts = weaponSpec->defaultIdleArmsBlendAmounts;
				}
			}
		}

	}

	//UActionComponent* actionComponent = UActionComponent::get(creature);
	//bool busy = actionComponent && actionComponent->isBusy();
	if (dualAxisAnimInstance) {
		dualAxisAnimInstance->resetAimBoneAmounts();
		dualAxisAnimInstance->resetTorsoSlotState();
		dualAxisAnimInstance->resetWeaponhandSnapEnabled();
		dualAxisAnimInstance->resetAimRotationEnabled();
		dualAxisAnimInstance->resetIdleArmsAmounts();
	}

	animInstance->weaponAnimSet = weaponAnimationSet;
}

void UEquipmentComponent::initActionSets() {

	UActionComponent* actionComponent = UActionComponent::get(GetOwner());
	if (!actionComponent) {
		return;
	}

	UActionSet* actionSet = nullptr;
	for (int i = 0; i < slots.Num(); i++) {
		UItemSpec* itemSpec = UResourceCache::getItemSpec(slots[i].itemKey);
		if (itemSpec) {
			if (itemSpec->IsA(UWeaponItemSpec::StaticClass())) {
				UWeaponItemSpec* weaponSpec = Cast< UWeaponItemSpec >(itemSpec);
				actionSet = weaponSpec->actionSet;
			}
		}
	}

	actionComponent->weaponActionSet = actionSet;
}

USceneComponent* UEquipmentComponent::getComponent(FString slotKey) {
	UWearableItemSpec* wearableItemSpec = Cast< UWearableItemSpec >( getEquippedItemSpec(slotKey, true) );
	if (wearableItemSpec) {
		UBodyComponent* body = UBodyComponent::get(GetOwner());
		if (body) {
			return body->getComponent(wearableItemSpec->slot);
		}
	}
	return nullptr;
}

void UEquipmentComponent::swapSlots(FString slotA, FString slotB) {
	FString originalItemA = getItemKey(slotA);
	int originalIndexA = getItemIndex(slotA);

	FString originalItemB = getItemKey(slotB);
	int originalIndexB = getItemIndex(slotB);

	setEquippedItem(slotA, originalItemB, originalIndexB);
	setEquippedItem(slotB, originalItemA, originalIndexA);

	markDirty();
}


void UEquipmentComponent::onEquipmentChanged() {
	markDirty();
	refresh();
}