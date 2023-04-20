#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RagdollComponent.h"

#include "RagdollAnimInstance.generated.h"

UCLASS()
class COREPLAY_API URagdollAnimInstance : public UAnimInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		AActor* owner = nullptr;

	UPROPERTY()
		USkeletalMeshComponent* meshComponent = nullptr;

	UPROPERTY()
		URagdollComponent* ragdoll = nullptr;

	UPROPERTY()
		TMap< FString, FTransform > boneTransforms;

	UPROPERTY()
		bool writingBoneTransforms = false;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float deltaTime) override;


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
		virtual FTransform getBoneTransform(FString boneKey);

};

