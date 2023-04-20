#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Item.h"
#include "ItemSpec.h"
#include "ItemSet.h"

#include "InventoryComponent.generated.h"


struct FItemSetItem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated)
		TArray< UItem* > items;

	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME_CONDITION(UInventoryComponent, items, COND_OwnerOnly);
	}

	virtual UItem* addItem(UItem* newItem);
	virtual UItem* addItem(FString itemSpecKey);
	virtual UItem* addItem(FString itemSpecKey, int quantity, int durability);
	virtual UItem* addItem(FItemSetItem& itemSetItem);

	virtual bool removeItem(FString itemSpecKey);
	virtual bool removeItem(UItem* item);
	virtual int removeItem(FString itemSpecKey, int quantity);
	virtual int removeItem(UItem* item, int quantity);


	virtual void applyItemSet(FItemSet& itemSet);

	virtual int countItem(FString itemSpecKey);
	virtual bool hasItem(FString itemSpecKey, int quantity = 1);

	virtual int getItemIndex(UItem* item);
	virtual FString getEquippedSlotKey(UItem* item);
	virtual bool isEquipped(UItem* item);
	virtual void unequipItem(UItem* item);
	virtual void equipItem(UItem* item, FString slotKey = "");
	virtual UItem* getEquippedItem(FString slotKey);

	static UInventoryComponent* get(AActor* actor);

};