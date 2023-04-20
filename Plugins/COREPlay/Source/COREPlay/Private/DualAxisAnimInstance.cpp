#include "DualAxisAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DualAxisCreature.h"
#include "ActionComponent.h"
#include "MathTools.h"
#include "Debug.h"

void UDualAxisAnimInstance::NativeUpdateAnimation(float deltaTime) {
	Super::NativeUpdateAnimation(deltaTime);

	ADualAxisCreature* creature = Cast<ADualAxisCreature>(owner);
	if (!creature) {
		return;
	}

	
	bool isAiming = creature->isAiming();
	altHanded = creature->altHanded;
	aimAnimPerc = UMathTools::lerpFloatByAmount(aimAnimPerc, isAiming ? 1.0f : 0.0f, deltaTime * 10.0f);
	weaponHandSnapPerc = UMathTools::lerpFloatByAmount(weaponHandSnapPerc, weaponHandSnapEnabled ? 1.0f : 0.0f, deltaTime * 10.0f);

	aimRotationPerc = UMathTools::lerpFloatByAmount(aimRotationPerc, aimRotationEnabled ? 1.0f : 0.0f, deltaTime * 10.0f);
	if (aimRotationEnabled) {

		FVector aimBodyLocation = creature->GetMesh()->GetSocketLocation("Spine1");
		FVector offset = isAiming ? FVector(0, 30, 40) : FVector(0,30,25);
		if (altHanded) {
			offset.Y *= -1.0f;
		}
		aimBodyLocation += FRotator(0.0f, creature->bodyYaw + 90.0f, 0.0f).RotateVector(offset);

		FRotator yawAndPitch = UMathTools::getYawAndPitch(aimBodyLocation, creature->aimAtPoint);
		aimRotation.X = UMathTools::clampPitch(UMathTools::getAngleDiff(creature->bodyYaw + 90.0f, yawAndPitch.Yaw), 90.0f);
		aimRotation.Y = UMathTools::clampPitch(yawAndPitch.Pitch, 90.0f);

	}
	else {
		aimRotation = FVector2D::ZeroVector;
	}

	float bodyAngleDiff = UMathTools::getAngleDiff(creature->controlRotation.Yaw, creature->bodyYaw + 90.0f);
	moveDirection = creature->inputMove.GetRotated(bodyAngleDiff) * movePerc;

	turnAmount = UMathTools::getAngleDiff(lastYaw, creature->bodyYaw);
	lastYaw = creature->bodyYaw;
	crouching = creature->crouched;
}

FString UDualAxisAnimInstance::addHand(FString value) {
	if (altHanded) {
		return value + "_LeftHanded";
	}
	return value + "_RightHanded";
}

FString UDualAxisAnimInstance::addLocomotionState(FString value) {
	if (crouching) {
		return value + "Crouched";
	}
	return value;
}

float UDualAxisAnimInstance::getAimBoneAmount(FString value) {

	if (altHanded) {
		if (value.Contains("Right")) {
			value = value.Replace(TEXT("Right"), TEXT("Left"));
		}
		else if (value.Contains("Left")) {
			value = value.Replace(TEXT("Left"), TEXT("Right"));
		}
	}

	if (aimBoneAmounts.Contains(value)) {
		return aimBoneAmounts[value] * aimRotation.Y;
	}
	return 0.0f;
}

void UDualAxisAnimInstance::resetAimBoneAmounts() {
	aimBoneAmounts = defaultAimBoneAmounts;
}

void UDualAxisAnimInstance::setTorsoSlotState(ESlotAnimState newState) {
	torsoSlotState = newState;
}

void UDualAxisAnimInstance::resetTorsoSlotState() {
	setTorsoSlotState(defaultTorsoSlotState);
}

void UDualAxisAnimInstance::setWeaponHandSnapEnabled(bool enabled) {
	weaponHandSnapEnabled = enabled;
}

void UDualAxisAnimInstance::resetWeaponhandSnapEnabled() {
	setWeaponHandSnapEnabled(defaultWeaponHandSnapEnabled);
}

void UDualAxisAnimInstance::setAimRotationEnabled(bool enabled) {
	aimRotationEnabled = enabled;
}

void UDualAxisAnimInstance::resetAimRotationEnabled() {
	setAimRotationEnabled(defaultAimRotationEnabled);
}

float UDualAxisAnimInstance::getIdleArmsBlendAmount(FString value) {

	if (altHanded) {
		if (value.Contains("Right")) {
			value = value.Replace(TEXT("Right"), TEXT("Left"));
		}
		else if (value.Contains("Left")) {
			value = value.Replace(TEXT("Left"), TEXT("Right"));
		}
	}

	if (idleArmsBlendAmounts.Contains(value)) {
		return idleArmsBlendAmounts[value];
	}
	return 0.0f;
}

void UDualAxisAnimInstance::resetIdleArmsAmounts() {
	idleArmsBlendAmounts = defaultIdleArmsBlendAmounts;
}


void UDualAxisAnimInstance::processPlayAnimStart(FPlayAnimationData& animData) {
	if (animData.slot.Contains("Torso") || animData.slot.Contains("Upper")) {
		if (animData.changeWeaponSnapEnabled) {
			setWeaponHandSnapEnabled(animData.weaponSnapEnabled);
		}
		if (animData.changeSlotState) {
			setTorsoSlotState(animData.slotState);
		}
		if (animData.changeAimRotationEnabled) {
			setAimRotationEnabled(animData.aimRotationEnabled);
		}
		for (auto& boneAmount : animData.aimBoneAmounts) {
			aimBoneAmounts.Add(boneAmount.Key, boneAmount.Value);
		}
	}
}

void UDualAxisAnimInstance::processPlayAnimEnd(FPlayAnimationData& animData) {
	if ((animData.slot.Contains("Torso") || animData.slot.Contains("Upper")) || animData.changeSlotState) {
		if (animData.changeWeaponSnapEnabled) {
			resetWeaponhandSnapEnabled();
		}
		if (animData.changeSlotState) {
			resetTorsoSlotState();
		}
		if (animData.changeAimRotationEnabled) {
			resetAimRotationEnabled();
		}
		resetAimBoneAmounts();
		resetIdleArmsAmounts();
	}
}