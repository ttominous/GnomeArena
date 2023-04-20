#include "ItemSpawner.h"
#include "ResourceCache.h"
#include "Debug.h"


AItemSpawner::AItemSpawner() {
	PrimaryActorTick.bCanEverTick = true;
}

void AItemSpawner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		return;
	}


	if (currentItem == nullptr) {
		dropTime -= DeltaTime;
		if (dropTime <= 0.0f) {
			dropTime = dropFrequency;
			if (dropItemClass) {
				FActorSpawnParameters params;
				params.bNoFail = true;
				currentItem = GetWorld()->SpawnActor<ADropItem>(dropItemClass, GetActorLocation(), GetActorRotation(), params);

				if (currentItem) {
					currentItem->type = type;
					currentItem->displaySpecKey = displaySpecKey;
					currentItem->pickupFXListKey = pickupFXListKey;
					currentItem->stats = stats;
					for (FItemSetItem& itemSetItem : itemSet.items) {

						UItemSpec* itemSpec = UResourceCache::getItemSpec(itemSetItem.itemSpecKey);
						if (!itemSpec || itemSetItem.quantity == 0) {
							continue;
						}
						UItem* addedItem = NewObject< UItem >(currentItem, itemSpec->itemClass);
						addedItem->itemSpecKey = itemSetItem.itemSpecKey;
						addedItem->quantity = itemSetItem.quantity;
						addedItem->durability = itemSetItem.durability;
						addedItem->initNew();
						currentItem->items.Add(addedItem, itemSetItem.equippedSlot);
					}

					currentItem->init();
				}
			}
		}
	}
}

FString AItemSpawner::getType() {
	return "ItemSpawner";
}

TArray< AItemSpawner* > AItemSpawner::getAll() {
	TArray< AItemSpawner* > results;
	if (APointOfInterest::instancesByType.Contains("ItemSpawner")) {
		for (APointOfInterest* pointOfInterest : APointOfInterest::instancesByType["ItemSpawner"].instances) {
			AItemSpawner* itemSpawner = Cast< AItemSpawner >(pointOfInterest);
			if (itemSpawner) {
				results.Add(itemSpawner);
			}
		}
	}
	return results;
}