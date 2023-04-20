#pragma once

#include "CoreMinimal.h"
#include "WearableItemSpec.h"
#include "AnimationSet.h"
#include "ActionSet.h"
#include "PlayAnimationData.h"
#include "DamageDetails.h"
#include "SlotAnimState.h"

#include "WeaponItemSpec.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UWeaponItemSpec : public UWearableItemSpec {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UAnimationSet* animationSet = nullptr;

	UPROPERTY(EditAnywhere)
		UActionSet* actionSet = nullptr;

	UPROPERTY(EditAnywhere)
		TMap< FString, FPlayAnimationData > animationDatas;

	UPROPERTY(EditAnywhere)
		FDamageDetails baseMeleeDamage;

	UPROPERTY(EditAnywhere)
		bool overrideMeleeDamageBox = false;

	UPROPERTY(EditAnywhere)
		FString meleeDamageBoxWeaponSocket = "";

	UPROPERTY(EditAnywhere)
		FVector meleeDamageBoxExtent = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector meleeDamageBoxOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FRotator meleeDamageBoxRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
		float aiMeleeRange = 100.0f;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > defaultAimBoneAmounts;

	UPROPERTY(EditAnywhere)
		bool defaultAimRotationEnabled = false;

	UPROPERTY(EditAnywhere)
		ESlotAnimState defaultTorsoSlotState;

	UPROPERTY(EditAnywhere)
		bool defaultWeaponHandSnapEnabled = false;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > defaultIdleArmsBlendAmounts;

};
