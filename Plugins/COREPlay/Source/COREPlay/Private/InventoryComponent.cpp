#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "ItemSet.h"
#include "ResourceCache.h"
#include "Debug.h"



UInventoryComponent::UInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

UItem* UInventoryComponent::addItem(UItem* newItem) {

	if (newItem == nullptr) {
		return nullptr;
	}

	UItemSpec* itemSpec = newItem->getItemSpec();
	if (itemSpec->stackable) {
		for (UItem* item : items) {
			if (itemSpec == item->getItemSpec()) {
				int stackLeft = itemSpec->maxStack - item->quantity;

				if (newItem->quantity <= stackLeft) {
					item->quantity += newItem->quantity;
					newItem->quantity = 0;
					return item;
				}
				else {
					item->quantity += stackLeft;
					newItem->quantity -= stackLeft;
				}
			}
		}
	}

	if (newItem->quantity == 0) {
		return nullptr;
	}

	UItem* addedItem = NewObject< UItem >(this, itemSpec->itemClass);
	newItem->copyTo(addedItem);
	items.Add(addedItem);
	AddReplicatedSubObject(addedItem);
	return addedItem;
}

UItem* UInventoryComponent::addItem(FString itemSpecKey) {
	UItemSpec* itemSpec = UResourceCache::getItemSpec(itemSpecKey);
	if (!itemSpec) {
		return nullptr;
	}
	return addItem(itemSpecKey,itemSpec->defaultQuantity,itemSpec->defaultDurability);
}

UItem* UInventoryComponent::addItem(FString itemSpecKey, int quantity, int durability) {
	UItemSpec* itemSpec = UResourceCache::getItemSpec(itemSpecKey);
	if (!itemSpec || quantity == 0) {
		return nullptr;
	}
	UItem* addedItem = NewObject< UItem >(this, itemSpec->itemClass);
	addedItem->itemSpecKey = itemSpecKey;
	addedItem->quantity = quantity;
	addedItem->durability = durability;
	addedItem->initNew();
	items.Add(addedItem);
	AddReplicatedSubObject(addedItem);
	return addedItem;
}

UItem* UInventoryComponent::addItem(FItemSetItem& itemSetItem) {
	UItem* addedItem = addItem(itemSetItem.itemSpecKey, itemSetItem.quantity, itemSetItem.durability);
	if (!addedItem) {
		return nullptr;
	}
	return addedItem;
}



bool UInventoryComponent::removeItem(FString itemSpecKey) {
	bool removedItems = false;
	for (int i = 0; i < items.Num(); i++) {
		if (items[i]->itemSpecKey.Equals(itemSpecKey)) {
			removeItem(items[i]);
			removedItems = true;
		}
	}
	return removedItems;
}

bool UInventoryComponent::removeItem(UItem* item) {
	if (item == nullptr) {
		return false;
	}
	unequipItem(item);
	items.Remove(item);
	RemoveReplicatedSubObject(item);
	return true;
}

int UInventoryComponent::removeItem(FString itemSpecKey, int quantity) {
	for (int i = 0; i < items.Num(); i++) {
		if (items[i]->itemSpecKey.Equals(itemSpecKey)) {
			
			if (items[i]->quantity == quantity) {
				removeItem(items[i]);
				return 0;
			}
			else if (items[i]->quantity > quantity) {
				items[i]->quantity -= quantity;
				return 0;
			}
			else {
				quantity -= items[i]->quantity;
				removeItem(items[i]);
				i--;
			}

		}
	}
	return quantity;
}

int UInventoryComponent::removeItem(UItem* item, int quantity) {
	if (item == nullptr) {
		return quantity;
	}
	if (item->quantity == quantity) {
		removeItem(item);
		return 0;
	}
	else if (item->quantity > quantity) {
		item->quantity -= quantity;
		return 0;
	}
	else {
		quantity -= item->quantity;
		removeItem(item);
		return quantity;
	}
}




void UInventoryComponent::applyItemSet(FItemSet& itemSet) {
	for (FItemSetItem& item : itemSet.items) {
		UItem* addedItem = addItem(item);
		if (addedItem == nullptr) {
			continue;
		}

		if (item.equipped) {
			equipItem(addedItem, item.equippedSlot);
		}
	}
}

int UInventoryComponent::countItem(FString itemSpecKey) {
	int count = 0;
	for (UItem* item : items) {
		if (item->itemSpecKey.Equals(itemSpecKey)) {
			count += item->quantity;
		}
	}
	return count;
}

bool UInventoryComponent::hasItem(FString itemSpecKey, int quantity) {
	return countItem(itemSpecKey) >= quantity;
}



int UInventoryComponent::getItemIndex(UItem* item) {
	if (item == nullptr) {
		return -1;
	}
	return items.Find(item);
}

FString UInventoryComponent::getEquippedSlotKey(UItem* item) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
	if (equipment == nullptr) {
		return "";
	}

	int itemIndex = getItemIndex(item);
	if (itemIndex == -1) {
		return "";
	}

	for (FItemSlot& slot : equipment->slots) {
		if (slot.itemIndex == itemIndex) {
			return slot.key;
		}
	}
	return "";
}

bool UInventoryComponent::isEquipped(UItem* item) {
	return getEquippedSlotKey(item).Len() > 0;
}

void UInventoryComponent::unequipItem(UItem* item) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
	if (equipment == nullptr) {
		return;
	}

	FString slotKey = getEquippedSlotKey(item);
	if (slotKey.Len() > 0) {
		equipment->unequipItem(slotKey);
	}
}

void UInventoryComponent::equipItem(UItem* item, FString slotKey) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
	if (equipment == nullptr || item == nullptr) {
		return;
	}

	if (slotKey.Len() == 0) {
		UWearableItemSpec* wearableSpec = Cast< UWearableItemSpec >(item->getItemSpec());
		if (wearableSpec == nullptr || wearableSpec->slot.Len() == 0) {
			return;
		}
		slotKey = wearableSpec->slot;
	}

	int itemIndex = getItemIndex(item);
	if (itemIndex == -1) {
		return;
	}

	equipment->setEquippedItem(slotKey, item->itemSpecKey, itemIndex);
	equipment->refresh();
}

UItem* UInventoryComponent::getEquippedItem(FString slotKey) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
	if (equipment == nullptr) {
		return nullptr;
	}

	for (FItemSlot& slot : equipment->slots) {
		if (slot.key.Equals(slotKey)) {
			if (slot.itemIndex < 0 || slot.itemIndex >= items.Num()) {
				return nullptr;
			}
			else {
				return items[slot.itemIndex];
			}
		}
	}
	return nullptr;
}


UInventoryComponent* UInventoryComponent::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}
	return Cast< UInventoryComponent >(actor->GetComponentByClass(UInventoryComponent::StaticClass()));
}