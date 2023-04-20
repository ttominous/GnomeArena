#include "RagdollAnimInstance.h"
#include "MathTools.h"

void URagdollAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	owner = TryGetPawnOwner();
}

void URagdollAnimInstance::NativeUpdateAnimation(float deltaTime) {
	Super::NativeUpdateAnimation(deltaTime);

	if (!owner) {
		return;
	}
	if (ragdoll == nullptr) {
		ragdoll = URagdollComponent::get(owner);
	}
	if (meshComponent == nullptr && ragdoll != nullptr) {
		meshComponent = ragdoll->meshComponent;
	}
	if (!meshComponent) {
		return;
	}

	if (boneTransforms.Num() != ragdoll->boneTransforms.Num()) {
		writingBoneTransforms = true;
		boneTransforms.Empty();
		for (auto entry : ragdoll->boneTransforms) {
			boneTransforms.Add(entry.boneName, entry.getTransform());
		}
		writingBoneTransforms = false;
	}

	for (auto entry : ragdoll->boneTransforms) {
		FTransform transform = entry.getTransform();
		boneTransforms[entry.boneName].SetLocation( UMathTools::lerpVectorByPerc(boneTransforms[entry.boneName].GetLocation(), transform.GetLocation(), deltaTime * 5.0f));
		boneTransforms[entry.boneName].SetRotation(UMathTools::lerpRotatorByPerc(boneTransforms[entry.boneName].GetRotation().Rotator(), transform.GetRotation().Rotator(), deltaTime * 5.0f).Quaternion());
	}
}

FTransform URagdollAnimInstance::getBoneTransform(FString boneKey) {
	if (writingBoneTransforms) {
		return FTransform();
	}
	return boneTransforms.Contains(boneKey) ? boneTransforms[boneKey] : FTransform();
}

