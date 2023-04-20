#include "DualAxisCreature.h"
#include "DualAxisAnimInstance.h"
#include "CameraManagerComponent.h"
#include "ActionComponent.h"
#include "BodyComponent.h"
#include "EquipmentComponent.h"
#include "MindComponent.h"
#include "MathTools.h"
#include "CollisionTools.h"
#include "Debug.h"

void ADualAxisCreature::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADualAxisCreature, crouched);
	DOREPLIFETIME(ADualAxisCreature, altHanded);
}

float ADualAxisCreature::getMoveRate() {
	if (isFalling()) {
		return 0.5f;
	}

	if (crouched) {
		if (isAiming()) {
			return 0.1f;
		}
		return 0.3f;
	}

	if (isAiming()) {
		return 0.2f;
	}
	return isSprinting() ? 1.0f : 0.6f;
}

bool ADualAxisCreature::isTryingToSprint() {
	if (!sprinting || aiming || isFalling() || !actionComponent->allowSprint() || primaryAttack || secondaryAttack) {
		return false;
	}
	FVector2D normalizedInputMove = inputMove;
	normalizedInputMove.Normalize();
	return normalizedInputMove.Y > 0.49f;
}

bool ADualAxisCreature::isSprinting() {
	return getActionComponent()->hasBaseAction("Sprint");
}

bool ADualAxisCreature::isAiming() {
	return aiming && getActionComponent()->allowAim();
}


void ADualAxisCreature::updateInputPayload() {
	clientToServerPayload.setBool("sprinting", sprinting);
	clientToServerPayload.setBool("aiming", aiming);
	clientToServerPayload.setBool("primaryAttack", primaryAttack);
	clientToServerPayload.setBool("secondaryAttack", secondaryAttack);
	clientToServerPayload.setBool("altHanded", altHanded);
	clientToServerPayload.setVector2D("inputMove", inputMove);
	clientToServerPayload.setVector("aimAtPoint", aimAtPoint);
	Super::updateInputPayload();
}



bool ADualAxisCreature::spawn() {
	if (!Super::spawn()) {
		return false;
	}
	//getActionComponent()->startBaseAction("DrawWeapon");
	return true;
}

void ADualAxisCreature::processInput(float deltaTime) {

	if (crouched) {
		if (isFalling()) {
			crouched = false;
		}
	}

	if (isTryingToSprint()) {
		if (!getActionComponent()->hasBaseAction("Sprint") && !getActionComponent()->isBusy()) {
			actionComponent->startBaseAction("Sprint");
			crouched = false;
		}
	}
	else {
		if (getActionComponent()->hasBaseAction("Sprint")) {
			actionComponent->stopBaseAction("Sprint");
		}
	}

	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		if (!getActionComponent()->isBusy() && primaryAttack) {
			UAction* action = getActionComponent()->getAction("PrimaryAttack");
			if (action) {
				if (action->canDoAction(this)) {
					sendPayload(FPayload::startAction("PrimaryAttack"));
				}
				else {
					UAction* reloadAction = getActionComponent()->getAction("ReloadWeapon");
					if (reloadAction && reloadAction->canDoAction(this)) {
						sendPayload(FPayload::startAction("ReloadWeapon"));
					}
				}
			}
		}
		else if (!getActionComponent()->isBusy() && secondaryAttack) {
			UAction* action = getActionComponent()->getAction("SecondaryAttack");
			if (action) {
				if (action->canDoAction(this)) {
					sendPayload(FPayload::startAction("SecondaryAttack"));
				}
			}
		}
	}

	Super::processInput(deltaTime);
	updateAimAtPoint(deltaTime);
}


void ADualAxisCreature::bindInput(UInputComponent* inputComponent, bool uiFocused) {
	if (!uiFocused) {
		inputComponent->BindAxis("MoveX", this, &ACreature::pressInputMoveX);
		inputComponent->BindAxis("MoveY", this, &ACreature::pressInputMoveY);
		inputComponent->BindAction("Jump", IE_Released, this, &ACreature::pressJump);
		inputComponent->BindAction("Sprint", IE_Pressed, this, &ADualAxisCreature::pressInputSprint);
		inputComponent->BindAction("Sprint", IE_Released, this, &ADualAxisCreature::releaseInputSprint);
		inputComponent->BindAction("Aim", IE_Pressed, this, &ADualAxisCreature::pressInputAim);
		inputComponent->BindAction("Aim", IE_Released, this, &ADualAxisCreature::releaseInputAim);
		inputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ADualAxisCreature::pressInputPrimaryAttack);
		inputComponent->BindAction("PrimaryAttack", IE_Released, this, &ADualAxisCreature::releaseInputPrimaryAttack);
		inputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ADualAxisCreature::pressInputSecondaryAttack);
		inputComponent->BindAction("SecondaryAttack", IE_Released, this, &ADualAxisCreature::releaseInputSecondaryAttack);
		inputComponent->BindAction("SwitchHands", IE_Released, this, &ADualAxisCreature::pressInputSwitchHands);
		inputComponent->BindAction("Crouch", IE_Released, this, &ADualAxisCreature::pressInputCrouch);
		inputComponent->BindAction("Reload", IE_Released, this, &ADualAxisCreature::pressInputReload);
	}
}


bool ADualAxisCreature::startJump() {
	requestPayload(FPayload::playAnimSequence(altHanded ? "Jump_LeftHanded" : "Jump_RightHanded"));
	return true;
}


void ADualAxisCreature::handlePayload(FPayload payload) {
	FString type = payload.getString("type");

	if (FPayload::TYPE_SWITCH_HANDS.Equals(type)) {
		altHanded = payload.getBool("altHanded", altHanded);

		if (getAnimInstance()) {
			UDualAxisAnimInstance* animInstance = Cast< UDualAxisAnimInstance >(getAnimInstance());
			animInstance->altHanded = altHanded;

			FString animKey = "SwitchHands";
			animKey = animInstance->addLocomotionState(animKey);
			animKey = animInstance->addHand(animKey);

			if (getMovePerc() < 0.1f && !isFalling()) {
				playAnimSequence(animKey, 1.0f, 0.0f, 0.0f, 0.5f);
			}
			else {
				playAnimSequence(animKey, 1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
		attachWeapons();
		getActionComponent()->startBaseAction("DrawWeapon");

		UCameraManagerComponent* cameraManager = UCameraManagerComponent::get(this);
		if (cameraManager) {
			cameraManager->snapToPoint(aimAtPoint);
		}

		return;
	}
	else if (FPayload::TYPE_PRESS_INPUT.Equals(type)) {

		if (payload.getString("inputKey").Equals("Crouch")) {
			bool preferredCrouch = payload.getBool("crouched", crouched);
			if (preferredCrouch && getActionComponent()->allowCrouch()) {
				crouched = true;
			}
			else {
				crouched = false;
			}
		}
		else if (payload.getString("inputKey").Equals("Reload")) {
			if (!getActionComponent()->isBusy()) {
				UAction* reloadAction = getActionComponent()->getAction("ReloadWeapon");
				if (reloadAction && reloadAction->canDoAction(this)) {
					sendPayload(FPayload::startAction("ReloadWeapon"));
					return;
				}
			}
		}
	}
	else if (type.Equals("WeaponHandSnapPerc")) {
		UDualAxisAnimInstance* animInstance = Cast< UDualAxisAnimInstance >(getAnimInstance());
		if (animInstance) {
			animInstance->weaponHandSnapEnabled = payload.getBool("Enabled", animInstance->weaponHandSnapEnabled);
			animInstance->weaponHandSnapPerc = payload.getNumeric("Perc", animInstance->weaponHandSnapPerc);
		}
		return;
	}
	Super::handlePayload(payload);
}


void ADualAxisCreature::receiveClientInput(FPayload payload) {
	sprinting = payload.getBool("sprinting");
	aiming = payload.getBool("aiming");
	primaryAttack = payload.getBool("primaryAttack");
	secondaryAttack = payload.getBool("secondaryAttack");

	altHanded = payload.getBool("altHanded");
	inputMove = payload.getVector2D("inputMove", inputMove);
	netAimAtPoint = payload.getVector("aimAtPoint", netAimAtPoint);

	Super::receiveClientInput(payload);
}

void ADualAxisCreature::attachWeapons() {
	TArray<UActorComponent*> components = this->GetComponentsByTag(UMeshComponent::StaticClass(), FName("EquippedWeapon"));
	for (UActorComponent* component : components) {
		UMeshComponent* meshComponent = Cast< UMeshComponent >(component);
		meshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, altHanded ? "Weapon_L" : "Weapon_R");
	}

	UEquipmentComponent* equipment = UEquipmentComponent::get(this);
	if (equipment) {
		equipment->markDirty();
	}
}

FVector ADualAxisCreature::getWeaponTip() {
	FVector weaponTip = GetMesh()->GetSocketLocation(altHanded ? "LeftHand" : "RightHand");
	TArray<UActorComponent*> components = GetComponentsByTag(UMeshComponent::StaticClass(), FName("EquippedWeapon"));
	if (components.Num() > 0) {
		UMeshComponent* meshComponent = Cast<UMeshComponent>(components[0]);
		if (meshComponent) {
			weaponTip = meshComponent->GetSocketLocation("Tip");
		}
	}
	return weaponTip;
}

USceneComponent* ADualAxisCreature::getWeaponComponent() {
	TArray<UActorComponent*> components = GetComponentsByTag(UMeshComponent::StaticClass(), FName("EquippedWeapon"));
	return components.Num() > 0 ? Cast<USceneComponent>(components[0]) : nullptr;
}

float ADualAxisCreature::getAimAccuracy() {
	float accuracy = 0.5f;
	if (crouched) {
		accuracy *= 1.25;
	}
	if (isAiming()) {
		accuracy *= 1.25;
	}
	float movePerc = getMovePerc();
	accuracy -= 0.5 * movePerc;

	accuracy *= getActionComponent()->getAimAccuracy();
	return FMath::Clamp(accuracy, 0.0f, 1.0f);
}

void ADualAxisCreature::updateAimAtPoint(float deltaTime) {

	if (!IsLocallyControlled()) {
		aimAtPoint = UMathTools::lerpVectorByPerc(aimAtPoint, netAimAtPoint, deltaTime * replicationSmoothRate);
		return;
	}

	if (isBot) {
		return;
	}

	UCameraManagerComponent* cameraManager = UCameraManagerComponent::get(this);
	if (!cameraManager) {
		return;
	}

	FVector cameraLocation = cameraManager->getCameraLocation();
	aimAtPoint = cameraLocation + cameraManager->getCameraRotation().RotateVector(FVector(9999,0,0));
	FVector originalAimAtPoint = aimAtPoint;
	FHitResult hit = UCollisionTools::getRayHit(GetWorld(), cameraLocation, aimAtPoint, ECollisionChannel::ECC_Pawn, this);
	if (hit.bBlockingHit) {
		aimAtPoint = hit.Location;

		FVector spine = GetMesh()->GetSocketLocation("Spine");

		FVector originalAimDirection = originalAimAtPoint - spine;
		originalAimDirection.Normalize();

		FVector aimDirection = aimAtPoint - spine;
		aimDirection.Normalize();

		if (originalAimDirection.Dot(aimDirection) < 0.33f ) {
			aimAtPoint = originalAimAtPoint;
		}

	}

	//UDebug::drawPoint(GetWorld(), aimAtPoint, 15.0f, FColor::Red, 0.05f);
}

FString ADualAxisCreature::getHitReactionAnim(FDamageDetails& damage) {
	return altHanded ? "HitReaction_LeftHanded" : "HitReaction_RightHanded";
}

void ADualAxisCreature::pressInputSprint() {
	playerInputPressed = true;
	if (!isBot) {
		sprinting = true;
	}
}

void ADualAxisCreature::releaseInputSprint() {
	playerInputPressed = true;
	if (!isBot) {
		sprinting = false;
	}
}

void ADualAxisCreature::pressInputAim() {
	playerInputPressed = true;
	if (!isBot) {
		aiming = true;
	}
}

void ADualAxisCreature::releaseInputAim() {
	playerInputPressed = true;
	if (!isBot) {
		aiming = false;
	}
}

void ADualAxisCreature::pressInputPrimaryAttack() {
	playerInputPressed = true;
	getActionComponent()->startProcessInputPress("PrimaryAttack");
	if (!isBot) {
		primaryAttack = true;
	}
}

void ADualAxisCreature::releaseInputPrimaryAttack() {
	playerInputPressed = true;
	getActionComponent()->startProcessInputRelease("PrimaryAttack");
	if (!isBot) {
		primaryAttack = false;
	}
}

void ADualAxisCreature::pressInputSecondaryAttack() {
	playerInputPressed = true;
	getActionComponent()->startProcessInputPress("SecondaryAttack");
	if (!isBot) {
		secondaryAttack = true;
	}
}

void ADualAxisCreature::releaseInputSecondaryAttack() {
	playerInputPressed = true;
	getActionComponent()->startProcessInputRelease("SecondaryAttack");
	if (!isBot) {
		secondaryAttack = false;
	}
}

void ADualAxisCreature::pressInputSwitchHands() {
	playerInputPressed = true;

	if (dead || !actionComponent->allowSwitchHands()) {
		return;
	}
	if (!isBot) {
		FPayload payload(FPayload::TYPE_SWITCH_HANDS);
		payload.setBool("altHanded", !altHanded);
		requestPayload(payload);
	}
}

void ADualAxisCreature::pressInputCrouch() {
	playerInputPressed = true;

	if (dead || !actionComponent->allowCrouch()) {
		return;
	}
	if (!isBot) {
		FPayload payload(FPayload::TYPE_PRESS_INPUT);
		payload.setString("inputKey", "Crouch");
		payload.setBool("crouched", !crouched);
		requestPayload(payload);
	}
}


void ADualAxisCreature::pressInputReload() {
	playerInputPressed = true;

	if (dead) {
		return;
	}
	if (!isBot) {
		FPayload payload(FPayload::TYPE_PRESS_INPUT);
		payload.setString("inputKey", "Reload");
		requestPayload(payload);
	}
}
