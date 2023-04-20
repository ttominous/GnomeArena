#pragma once

#include "CoreMinimal.h"
#include "SlotAnimState.h"

#include "PlayAnimationData.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FPlayAnimationData {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString animSequenceKey;

	UPROPERTY(EditAnywhere)
		UAnimSequence* animSequence = nullptr;

	UPROPERTY(EditAnywhere)
		float playRate = 1.0f;

	UPROPERTY(EditAnywhere)
		float startTime = 0.0f;

	UPROPERTY(EditAnywhere)
		float blendIn = 0.0f;

	UPROPERTY(EditAnywhere)
		float blendOut = 0.0f;

	UPROPERTY(EditAnywhere)
		int loops = 1;

	UPROPERTY(EditAnywhere)
		FString slot = "DefaultSlot";

	UPROPERTY(EditAnywhere)
		bool addHandIndicator = false;

	UPROPERTY(EditAnywhere)
		bool addLocomotionStateIndicator = false;

	UPROPERTY(EditAnywhere)
		ESlotAnimState slotState;

	UPROPERTY(EditAnywhere)
		bool changeSlotState = false;

	UPROPERTY(EditAnywhere)
		bool weaponSnapEnabled;

	UPROPERTY(EditAnywhere)
		bool changeWeaponSnapEnabled = false;

	UPROPERTY(EditAnywhere)
		bool aimRotationEnabled;

	UPROPERTY(EditAnywhere)
		bool changeAimRotationEnabled = false;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > aimBoneAmounts;
};