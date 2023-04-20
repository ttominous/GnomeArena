#pragma once

#include "CoreMinimal.h"

#include "VesselSpec.generated.h"

class UVesselAction;
class UAction;
class UActionSet;

USTRUCT(BlueprintType)
struct COREPLAY_API FVesselAccess {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FVector offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector offsetBounds = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		FVector entryLocation;

	UPROPERTY(EditAnywhere)
		float entryYaw = 0.0f;

	UPROPERTY(EditAnywhere)
		UAnimSequence* entryAnimation = nullptr;

	UPROPERTY(EditAnywhere)
		FVector exitLocation;

	UPROPERTY(EditAnywhere)
		float exitYaw = 0.0f;

	UPROPERTY(EditAnywhere)
		UAnimSequence* exitAnimation = nullptr;

};


USTRUCT(BlueprintType)
struct COREPLAY_API FVesselSlot {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		TArray< FVesselAccess > accesses;

	UPROPERTY(EditAnywhere)
		FVector offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		float yaw = 0.0f;

	UPROPERTY(EditAnywhere)
		UAnimSequence* loopingAnimation = nullptr;

	UPROPERTY(EditAnywhere)
		UAction* mainAction = nullptr;

	UPROPERTY(EditAnywhere)
		TArray< UActionSet* > actionSets;

};


UCLASS(Blueprintable)
class COREPLAY_API UVesselSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, FVesselSlot > slots;

	UPROPERTY()
		TMap< FString, int > slotMap;

	UPROPERTY(EditAnywhere)
		bool drawDebug = false;

	virtual int getSlotIndex(FString slotKey);

};
