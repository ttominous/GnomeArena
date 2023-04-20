#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "VesselSpec.h"
#include "TargetDetail.h"
#include "XMLTools.h"

#include "VesselComponent.generated.h"

USTRUCT(BlueprintType)
struct COREPLAY_API FVesselSlotAccessibility {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool accessible;

	UPROPERTY()
		TArray< bool > accessibleAccesses;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UVesselComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UVesselSpec* spec;

	UPROPERTY()
		TArray< AActor* > occupancies;

	UPROPERTY()
		TArray< FVesselSlotAccessibility > accessibilities;

	UPROPERTY()
		float accessibilityUpdateTimer = 0.0f;

	UVesselComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void populateOccupancies();

	virtual int getSlotIndex(FString slotKey);
	virtual TArray< FString > getOpenSlotKeys();
	virtual bool isOccupied(FString slotKey);
	virtual bool isOccupiedBy(FString slotKey, AActor* actor);
	virtual void occupySlot(FString slotKey, AActor* actor);
	virtual void leaveSlot(FString slotKey, AActor* actor);
	virtual FTransform getSlotTransform(FString slotKey);
	virtual UAction* getSlotAction(FString slotKey);
	virtual FTransform getAccessTransform(FString slotKey, int accessIndex, bool asExit);
	virtual FString getNearestOpenSlotKey(FVector location);
	virtual int getNearestAccess(FString slotKey, FVector location, bool asExit);

	virtual bool isSlotAccessible(FString slotKey);
	virtual bool isAccessAccessible(FString slotKey, int accessIndex);
	virtual void updateAccessibilities();

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);


	static UVesselComponent* get(UObject* object);

};