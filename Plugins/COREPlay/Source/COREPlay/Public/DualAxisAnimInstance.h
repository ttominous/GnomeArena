#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CreatureAnimInstance.h"
#include "AnimationSet.h"
#include "SlotAnimState.h"

#include "DualAxisAnimInstance.generated.h"

class UPawnMovementComponent;

UCLASS()
class COREPLAY_API UDualAxisAnimInstance : public UCreatureAnimInstance {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool altHanded = false;

	UPROPERTY(BlueprintReadWrite)
		FVector2D moveDirection = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
		FVector2D aimRotation = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
		float aimRotationPerc = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool aimRotationEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool defaultAimRotationEnabled = false;

	UPROPERTY(BlueprintReadWrite)
		float aimAnimPerc = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector offHandOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ESlotAnimState torsoSlotState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ESlotAnimState defaultTorsoSlotState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool weaponHandSnapEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool defaultWeaponHandSnapEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float weaponHandSnapPerc = 0.0f;

	UPROPERTY()
		float lastYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float turnAmount = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		bool crouching = false;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > aimBoneAmounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap< FString, float > defaultAimBoneAmounts;


	UPROPERTY(EditAnywhere)
		TMap< FString, float > idleArmsBlendAmounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap< FString, float > defaultIdleArmsBlendAmounts;

	virtual void NativeUpdateAnimation(float deltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		FString addHand(FString value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		FString addLocomotionState(FString value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		float getAimBoneAmount(FString value);
	virtual void resetAimBoneAmounts();

	virtual void setTorsoSlotState(ESlotAnimState newState);
	virtual void resetTorsoSlotState();

	virtual void setWeaponHandSnapEnabled(bool enabled);
	virtual void resetWeaponhandSnapEnabled();

	virtual void setAimRotationEnabled(bool enabled);
	virtual void resetAimRotationEnabled();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		float getIdleArmsBlendAmount(FString value);
	virtual void resetIdleArmsAmounts();

	virtual void processPlayAnimStart(FPlayAnimationData& animData) override;
	virtual void processPlayAnimEnd(FPlayAnimationData& animData) override;
};

