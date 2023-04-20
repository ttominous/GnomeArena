#include "DropItem.h"
#include "Creature.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "StatsComponent.h"
#include "ActionComponent.h"
#include "WeaponItemSpec.h"
#include "ResourceCache.h"
#include "Debug.h"

int ADropItem::lastID = 0;
TArray<ADropItem*> ADropItem::instances;

ADropItem* ADropItem::getByID(int id) {
	for (ADropItem* dropItem : instances) {
		if (dropItem != nullptr && dropItem->id == id) {
			return dropItem;
		}
	}
	return nullptr;
}


ADropItem::ADropItem() {
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
}

void ADropItem::BeginPlay() {
	Super::BeginPlay();
	if (!initialized) {
		init();
	}
}

void ADropItem::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	Super::EndPlay(EndPlayReason);
}


void ADropItem::init() {
	if (id == -1) {
		id = lastID;
		lastID++;
	}
	if (!instances.Contains(this)) {
		instances.Add(this);
	}
	build();
	initialized = true;
}

void ADropItem::build() {

	UStaticMeshComponent* staticMeshComponent = Cast< UStaticMeshComponent >(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (staticMeshComponent == nullptr) {
		return;
	}

	if (displaySpecKey.Len() > 0) {

		if (type == EDropItemType::WEAPON) {
			UItemSpec* itemSpec = UResourceCache::getItemSpec(displaySpecKey);
			if (itemSpec){
				staticMeshComponent->SetStaticMesh(itemSpec->dropItemMesh);
				staticMeshComponent->SetRelativeTransform(itemSpec->dropItemTransform);
			}
		}

	}

}

void ADropItem::processOverlap(AActor* actor) {

	if (actor == nullptr || !actor->IsA(ACreature::StaticClass()) || GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		return;
	}

	ACreature* creature = Cast< ACreature >(actor);
	if (creature->dead) {
		return;
	}

	bool weaponEquipped = false;
	bool statsChanged = false;

	UInventoryComponent* inventory = UInventoryComponent::get(creature);
	if (inventory) {
		for (auto& item : items) {
			UItem* addedItem = inventory->addItem(item.Key);
			if (addedItem && item.Value.Len() > 0) {
				inventory->equipItem(addedItem, item.Value);
				if (addedItem->getItemSpec()->IsA(UWeaponItemSpec::StaticClass())) {
					weaponEquipped = true;
				}
			}
		}
	}
	else {

		UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
		for (auto& item : items) {
			if (item.Value.Len() > 0) {
				equipment->setEquippedItem(item.Value, item.Key->itemSpecKey, -1);

				UItemSpec* itemSpec = UResourceCache::getItemSpec(item.Key->itemSpecKey);
				if (itemSpec && itemSpec->IsA(UWeaponItemSpec::StaticClass())) {
					weaponEquipped = true;
				}

			}
		}
	}

	UStatsComponent* creatureStats = UStatsComponent::get(creature);
	for (auto& stat : stats) {
		creatureStats->addStatValue(stat.Key, stat.Value);
		statsChanged = true;
	}

	sendPickup(creature,weaponEquipped,statsChanged);
}

void ADropItem::sendPickup_Implementation(ACreature* creature, bool equippedWeapon, bool statsChanged) {
	processLocalPickup(creature, equippedWeapon, statsChanged);
}

void ADropItem::processLocalPickup(ACreature* creature, bool equippedWeapon, bool statsChanged) {
	if (!creature) {
		return;
	}

	if (statsChanged) {
		creature->refreshMaterials();
	}

	UFXList* fxList = UResourceCache::getFXList(pickupFXListKey);
	if (fxList) {
		fxList->execute(creature, creature->GetActorLocation(), creature->bodyYaw, FVector(1.0f));
	}

	if (equippedWeapon) {
		creature->getActionComponent()->startBaseAction("DrawWeapon");
	}

	Destroy();

}


void ADropItem::writeXML(FXMLBuilderNode& rootNode) {

}

void ADropItem::readXML(FXmlNode* rootNode) {
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		
	}
}

void ADropItem::destroyAll() {
	TArray<ADropItem*> instancesCopy = instances;
	for (ADropItem* sceneObject : instancesCopy) {
		if (sceneObject) {
			sceneObject->Destroy();
		}
	}
	instances.Empty();
}

ADropItem* ADropItem::spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw, bool asGhost) {
	if (spawnClass == nullptr) {
		UDebug::error("Could not spawn DropItem, because its Class was missing.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	ADropItem* dropItem = world->SpawnActor<ADropItem>(spawnClass, position, FRotator(0.0f, yaw, 0.0f), params);
	if (dropItem) {
		dropItem->init();
		dropItem->SetActorLocation(position);
		dropItem->SetActorRotation(FRotator(0.0f, yaw, 0.0f));
	}
	else {
		UDebug::error("Spawn drop item, " + spawnClass->GetName() + " failed!");
	}
	return dropItem;
}