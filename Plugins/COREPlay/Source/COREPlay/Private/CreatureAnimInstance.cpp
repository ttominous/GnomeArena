#include "CreatureAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Creature.h"
#include "MathTools.h"

void UCreatureAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	initialize();
}

UPawnMovementComponent* UCreatureAnimInstance::getMovementComponent() {
	if (movementComponent == nullptr) {
		movementComponent = owner->GetMovementComponent();
	}
	return movementComponent;
}

void UCreatureAnimInstance::NativeUpdateAnimation(float deltaTime) {
	Super::NativeUpdateAnimation(deltaTime);

	ACreature* creature = Cast<ACreature>(owner);
	if (!creature) {
		return;
	}

	if (getMovementComponent()) {
		isFalling = movementComponent->IsFalling();
		movePerc = creature->getMovePerc();
		isMoving = movePerc > 0.01f;
	}

	if (lookAtPerc > 0.01f) {
		float relativeLookAtPerc = lookAtPerc;

		FVector headLocation = creature->GetMesh()->GetSocketLocation("Head");
		float dist = FVector::Dist(headLocation, lookAtPoint);
		if (dist < 100.0f) {
			relativeLookAtPerc *= dist / 100.0f;
		}

		finalLookAtPerc = UMathTools::lerpFloatByAmount(finalLookAtPerc, relativeLookAtPerc, deltaTime * 3.5f);
		finalLookAtPoint = UMathTools::lerpVectorByPerc(finalLookAtPoint, lookAtPoint, deltaTime);
	}
	else {
		finalLookAtPerc = UMathTools::lerpFloatByAmount(finalLookAtPerc, lookAtPerc, deltaTime * 3.5f);
	}

	if (hipsTransform.active) {
		hipsTransform.time -= deltaTime;
		if (hipsTransform.time <= 0.0f) {
			hipsTransform.active = false;
		}
	}

}


UAnimSequence* UCreatureAnimInstance::getSequence(FString animKey) {
	if (weaponAnimSet != nullptr) {
		UAnimSequence* asset = weaponAnimSet->getSequence(animKey);
		if (asset) {
			return asset;
		}
	}
	return baseAnimSet == nullptr ? nullptr : baseAnimSet->getSequence(animKey);
}

UBlendSpace* UCreatureAnimInstance::getBlendSpace(FString animKey) {
	if (weaponAnimSet != nullptr) {
		UBlendSpace* asset = weaponAnimSet->getBlendSpace(animKey);
		if (asset) {
			return asset;
		}
	}
	return baseAnimSet == nullptr ? nullptr : baseAnimSet->getBlendSpace(animKey);
}

UBlendSpace1D* UCreatureAnimInstance::getBlendSpace1D(FString animKey) {
	if (weaponAnimSet != nullptr) {
		UBlendSpace1D* asset = weaponAnimSet->getBlendSpace1D(animKey);
		if (asset) {
			return asset;
		}
	}
	return baseAnimSet == nullptr ? nullptr : baseAnimSet->getBlendSpace1D(animKey);
}

UAnimMontage* UCreatureAnimInstance::getMontage(FString animKey) {
	if (weaponAnimSet != nullptr) {
		UAnimMontage* asset = weaponAnimSet->getMontage(animKey);
		if (asset) {
			return asset;
		}
	}
	return baseAnimSet == nullptr ? nullptr : baseAnimSet->getMontage(animKey);
}

float UCreatureAnimInstance::convertToSequenceTime(FString animKey, float time) {
	UAnimSequence* sequence = getSequence(animKey);
	if (sequence) {
		return time * sequence->GetPlayLength();
	}
	return 0.0f;
}

void UCreatureAnimInstance::setLookAt(FVector position, float amount) {
	lookAtPerc = amount;
	lookAtPoint = position;
	if (amount <= 0.01f) {
		finalLookAtPoint = lookAtPoint;
	}
}

void UCreatureAnimInstance::setEyeTurn(FVector2D amount, float perc) {
	eyeTurnAmount = amount;
	eyeTurnPerc = perc;
}

void UCreatureAnimInstance::clearLookAt(bool immediate) {
	lookAtPerc = 0.0f;
	if (immediate) {
		finalLookAtPerc = 0.0f;
	}
}

void UCreatureAnimInstance::initialize() {
	owner = TryGetPawnOwner();
	if (baseAnimSet == nullptr) {
		//baseAnimSet = UResourceCache::getAnimationSet(baseAnimSetKey);
	}
}

void UCreatureAnimInstance::processPlayAnimStart(FPlayAnimationData& animData) {

}

void UCreatureAnimInstance::processPlayAnimEnd(FPlayAnimationData& animData) {

}

UAnimMontage* UCreatureAnimInstance::getLastAnimation(FString slot) {
	return lastAnimations.Contains(slot) ? lastAnimations[slot] : nullptr;
}

bool UCreatureAnimInstance::isPlayingAnimation(FString slot) {
	UAnimMontage* lastAnimation = getLastAnimation(slot);
	if (!lastAnimation) {
		return false;
	}
	return IsPlayingSlotAnimation(lastAnimation,FName(*slot));
}

void UCreatureAnimInstance::updateAnimationImmediately() {
	ACreature* creature = Cast<ACreature>(owner);
	if (!creature) {
		return;
	}

	USkeletalMeshComponent* mesh = creature->GetMesh();
	mesh->bEnableUpdateRateOptimizations = false;
	UpdateAnimation(0.001f, false);
	mesh->TickAnimation(0.001f, false);
	mesh->TickPose(0.001f, true);
	mesh->RefreshBoneTransforms();
	mesh->bEnableUpdateRateOptimizations = true;
}

void UCreatureAnimInstance::fadeOutHipsLocation(float time) {

	if (time <= 0.0f) {
		return;
	}

	ACreature* creature = Cast<ACreature>(owner);
	if (!creature) {
		return;
	}

	hipsTransform.location = creature->GetMesh()->GetSocketLocation("Hips");
	hipsTransform.time = time;
	hipsTransform.interval = time;
	hipsTransform.active = true;
}

float UCreatureAnimInstance::getHipsTransformPerc() {
	return hipsTransform.active ? (hipsTransform.time / hipsTransform.interval) : 0.0f;
}

FVector UCreatureAnimInstance::getHipsTransformLocation() {
	return hipsTransform.location;
}