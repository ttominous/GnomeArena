#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimationSet.h"
#include "PlayAnimationData.h"

#include "CreatureAnimInstance.generated.h"

class UPawnMovementComponent;

USTRUCT(BlueprintType)
struct COREPLAY_API FAnimBoneTransform {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool active = false;

	UPROPERTY()
		FVector location;

	UPROPERTY()
		float time;

	UPROPERTY()
		float interval;

};

UCLASS()
class COREPLAY_API UCreatureAnimInstance : public UAnimInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		APawn* owner = nullptr;

	UPROPERTY(BlueprintReadWrite)
		bool isMoving = false;

	UPROPERTY(BlueprintReadWrite)
		float movePerc = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		bool isFalling = false;

	UPROPERTY(BlueprintReadWrite)
		bool isArmed = false;

	UPROPERTY()
		FVector lookAtPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector finalLookAtPoint;

	UPROPERTY()
		float lookAtPerc = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D eyeTurnAmount = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float eyeTurnPerc = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float finalLookAtPerc = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString animKeyMove = "Move";

	UPROPERTY(EditAnywhere)
		UAnimationSet* baseAnimSet = nullptr;

	UPROPERTY(EditAnywhere)
		FString baseAnimSetKey = "";

	UPROPERTY(EditAnywhere)
		UAnimationSet* weaponAnimSet = nullptr;

	UPROPERTY()
		UPawnMovementComponent* movementComponent = nullptr;

	UPROPERTY()
		TMap< FString, UAnimMontage* > lastAnimations;

	UPROPERTY(BlueprintReadWrite)
		FAnimBoneTransform hipsTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool inVessel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool exitingVessel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool enteringVessel = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* vesselEntryAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* vesselLoopAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequence* vesselExitAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector vesselOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector animOffset;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaTime) override;

	UPawnMovementComponent* getMovementComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual UAnimSequence* getSequence(FString animKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual UBlendSpace* getBlendSpace(FString animKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual UBlendSpace1D* getBlendSpace1D(FString animKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual UAnimMontage* getMontage(FString animKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual float convertToSequenceTime(FString animKey, float time);

	virtual void setLookAt(FVector position, float amount);
	virtual void setEyeTurn(FVector2D amount, float perc);
	virtual void clearLookAt(bool immediate);
	virtual void initialize();

	virtual void processPlayAnimStart(FPlayAnimationData& animData);
	virtual void processPlayAnimEnd(FPlayAnimationData& animData);
	virtual UAnimMontage* getLastAnimation(FString slot);
	virtual bool isPlayingAnimation(FString slot);

	virtual void updateAnimationImmediately();

	virtual void fadeOutHipsLocation(float time);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual float getHipsTransformPerc();

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual FVector getHipsTransformLocation();

};

