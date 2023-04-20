#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "ItemSlot.h"

#include "EquipmentComponent.generated.h"

class UItemSpec;
class UWeaponItemSpec;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UEquipmentComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated, ReplicatedUsing = OnRep_Equipped)
		TArray< FItemSlot > slots;

	UEquipmentComponent();

	static UEquipmentComponent* get(AActor* actor);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UEquipmentComponent, slots);
	}

	virtual bool hasSlot(FString slotKey);
	virtual int getSlotIndex(FString slotKey);
	virtual int getItemIndex(FString slotKey);
	virtual FString getItemKey(FString slotKey);
	virtual bool setEquippedItem(FString slotKey, FString itemKey, int itemIndex);
	virtual bool unequipItem(FString slotKey);
	virtual bool unequipAllItems();
	virtual bool hasEquippedItem(FString slotKey);
	virtual UItemSpec* getEquippedItemSpec(FString slotKey, bool considerTemporary = false);
	virtual UWeaponItemSpec* getEquippedWeapon(FString preferredSlotKey, bool allowAnyWeapon = false, bool considerTemporary = false);

	virtual bool hasTemporaryItem(FString slotKey);
	virtual bool setTemporaryItem(FString slotKey, FString itemKey);
	virtual bool clearTemporaryItem(FString slotKey);
	virtual bool clearAllTemporaryItems(FString slotKey);

	virtual void onEquipmentChanged();
	virtual void markDirty();

	virtual void postProcessEquipItem(FString slotKey, FString itemKey);
	virtual void postProcessUnEquipItem(FString slotKey,FString itemKey);
	virtual void refresh();
	virtual void initAnimInstance();
	virtual void initActionSets();

	virtual USceneComponent* getComponent(FString slotKey);
	virtual void swapSlots(FString slotA, FString slotB);

	UFUNCTION()
	void OnRep_Equipped() {
		onEquipmentChanged();
	}
};