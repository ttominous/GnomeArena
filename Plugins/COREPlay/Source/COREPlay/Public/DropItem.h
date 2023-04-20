#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XMLTools.h"

#include "DropItem.generated.h"


UENUM(BlueprintType)
enum class EDropItemType : uint8 {
	UNKNOWN = 0					UMETA(DisplayName = "Unknown"),
	WEAPON = 1					UMETA(DisplayName = "Weapon"),
	HEALTH = 2					UMETA(DisplayName = "Health"),
	ARMOR = 3					UMETA(DisplayName = "Armor"),
};

class UItem;

UCLASS()
class COREPLAY_API ADropItem : public AActor {
	GENERATED_BODY()

public:

	static TArray< ADropItem* > instances;

	UPROPERTY()
		bool initialized = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	static int lastID;

	UPROPERTY(replicated)
		int id = -1;

	UPROPERTY()
		EDropItemType type;

	UPROPERTY(replicated)
		FString pickupFXListKey;

	UPROPERTY()
		TMap< UItem*, FString > items;

	UPROPERTY()
		TMap< FString, int > stats;

	UPROPERTY(replicated)
		FString displaySpecKey;

	static ADropItem* getByID(int id);

	ADropItem();


	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ADropItem, id);
		DOREPLIFETIME(ADropItem, displaySpecKey);
		DOREPLIFETIME(ADropItem, pickupFXListKey);
	}


	virtual void init();
	virtual void build();

	UFUNCTION(BlueprintCallable)
		virtual void processOverlap(AActor* actor);

	UFUNCTION(NetMulticast, Reliable)
		void sendPickup(ACreature* creature, bool equippedWeapon, bool statsChanged);
	virtual void processLocalPickup(ACreature* creature, bool equippedWeapon, bool statsChanged);

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

	static void destroyAll();
	static ADropItem* spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw, bool asGhost);

};