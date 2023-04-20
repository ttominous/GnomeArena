#include "CameraManagerComponent.h"
#include "Creature.h"
#include "DualAxisCreature.h"
#include "ComplexPlayerController.h"
#include "MathTools.h"
#include "ActionComponent.h"
#include "CollisionTools.h"
#include "Team.h"
#include "Debug.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

TArray< UCameraManagerComponent* > UCameraManagerComponent::instances;

UCameraManagerComponent::UCameraManagerComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UCameraManagerComponent::BeginPlay() {
	Super::BeginPlay();
	instances.Add(this);
}

void UCameraManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	Super::EndPlay(EndPlayReason);
}


void UCameraManagerComponent::DestroyComponent(bool bPromoteChildren) {

	if (camera) {
		camera->DestroyComponent();
	}
	if (cameraArm) {
		cameraArm->DestroyComponent();
	}

	Super::DestroyComponent(bPromoteChildren);
}


void UCameraManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!spawned) {
		spawn();
		spawned = true;
	}

	update(DeltaTime);

	if (justSwitchedModes) {
		justSwitchedModes = false;
	}
}

void UCameraManagerComponent::spawn() {
	initCameraArm();
	initCamera();
	switchMode(defaultMode);
}

void UCameraManagerComponent::initCameraArm() {
	FTransform transform;
	cameraArm = Cast< USpringArmComponent>(GetOwner()->AddComponentByClass(USpringArmComponent::StaticClass(), false, transform, false));
	if (!cameraArm) {
		UDebug::error("Could not instantiate USpringArmComponent!");
		return;
	}
	cameraArm->bDoCollisionTest = collisionEnabled;
	cameraArm->bUsePawnControlRotation = false;
}

void UCameraManagerComponent::initCamera() {

	if (!cameraArm) {
		return;
	}
	FTransform transform;
	camera = Cast< UCameraComponent>(GetOwner()->AddComponentByClass(UCameraComponent::StaticClass(), false, transform, false));
	if (!camera) {
		UDebug::error("Could not instantiate UCameraComponent!");
		return;
	}

	camera->SetFieldOfView(60.0f);
	camera->AttachToComponent(cameraArm, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void UCameraManagerComponent::switchMode(ECameraMode newMode) {
	updateController();
	snapToPointEnabled = false;

	ACreature* controlledCreature = nullptr;
	if (GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* pawn = Cast< ACreature >(GetOwner());
		pawn->resetControlRotation();
		targetYaw = pawn->controlRotation.Yaw;
		targetPitch = pawn->controlRotation.Pitch;
		controlledCreature = pawn;
		controlledCreature->showToOwner();
	}

	if (newMode == ECameraMode::CHARACTER_FACE_LIMITED || newMode == ECameraMode::CHARACTER_BODY_LIMITED) {
		canTurnYaw = false;
		canTurnPitch = false;
		turnSpeed = 1.0f;
		canZoom = false;
		showCursor = false;
	}
	else if (newMode == ECameraMode::THIRD_PERSON_FREE) {
		targetArmLength = defaultArmLength;
		canTurnYaw = true;
		canTurnPitch = true;
		turnSpeed = 1.0f;
		canZoom = false;
		showCursor = false;
	}
	else if (newMode == ECameraMode::THIRD_PERSON_SHOOTER) {
		targetArmLength = 150.0f;
		canTurnYaw = true;
		canTurnPitch = true;
		turnSpeed = 1.0f;
		canZoom = false;
		showCursor = false;
	}
	else if (newMode == ECameraMode::TOP_DOWN) {
		targetArmLength = 500.0f;
		targetPitch = -45.0f;
		turnSpeed = 1.0f;
		canTurnYaw = true;
		canTurnPitch = false;
		canZoom = false;
		showCursor = false;
	}
	else if (newMode == ECameraMode::RTS) {
		targetPitch = -45.0f;
		canTurnYaw = true;
		canTurnPitch = true;
		canZoom = true;
		targetArmLength = 800.0f;
		minTargetArmLength = 100.0f;
		maxTargetArmLength = 1500.0f;
		targetArmLengthZoomStep = 50.0f;
		turnSpeed = 5.0f;
		setCollisionEnabled(false);
		showCursor = true;
	}
	mode = newMode;
	justSwitchedModes = true;

	if (controller != nullptr && controller->IsA(AComplexPlayerController::StaticClass())) {
		AComplexPlayerController* playerController = Cast< AComplexPlayerController >(controller);
		playerController->gameShowCursor = showCursor;
		playerController->updateShowCursor();
	}
}

void UCameraManagerComponent::setDefaultMode(ECameraMode newMode) {
	defaultMode = newMode;
}

void UCameraManagerComponent::switchDefaultMode() {
	switchMode(defaultMode);
}

void UCameraManagerComponent::setCollisionEnabled(bool enabled) {
	collisionEnabled = enabled;
	if (cameraArm) {
		cameraArm->bDoCollisionTest = collisionEnabled;
	}
}

void UCameraManagerComponent::setShowCursor(bool inShowCursor) {
	if (controller == nullptr) {
		updateController();
	}
	if (controller != nullptr) {
		controller->bShowMouseCursor = inShowCursor;
	}
}

void UCameraManagerComponent::update(float deltaTime) {


	updateController();
	if (controller == nullptr) {
		return;
	}

	if (!controller->IsLocalController()) {
		return;
	}

	if (mode == ECameraMode::THIRD_PERSON_FREE) {
		updateThirdPersonFreeCamera(deltaTime);
	}
	else if (mode == ECameraMode::THIRD_PERSON_SHOOTER) {
		updateThirdPersonShooterCamera(deltaTime);
	}
	else if (mode == ECameraMode::TOP_DOWN) {
		updateTopDownCamera(deltaTime);
	}
	else if (mode == ECameraMode::CHARACTER_FACE_LIMITED || mode == ECameraMode::CHARACTER_BODY_LIMITED) {
		updateCharacterLimitedCamera(deltaTime);
	}
	else if (mode == ECameraMode::CHARACTER_DEAD) {
		updateCharacterDeadCamera(deltaTime);
	}
	else if (mode == ECameraMode::RTS) {
		updateRTSCamera(deltaTime);
	}
	lastLocation = cameraArm->GetComponentLocation();
	lastRotation = cameraArm->GetComponentRotation();
	lastTargetArmLength = cameraArm->TargetArmLength;

	if (needsSyncOwner) {
		needsSyncOwner = false;
		syncControlRotation();
	}
}

void UCameraManagerComponent::updateController() {
	controller = nullptr;
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn) {
		controller = Cast< APlayerController >(pawn->Controller);
	}
}

void UCameraManagerComponent::updateThirdPersonFreeCamera(float deltaTime) {

	FVector location;
	if (GetOwner()->IsA(ACreature::StaticClass())) {
		location = ((ACreature*)GetOwner())->GetMesh()->GetComponentLocation();
		location.Z = ((ACreature*)GetOwner())->GetMesh()->GetSocketLocation(FName(*defaultBoneName)).Z;
	}
	else {
		location = GetOwner()->GetActorLocation() + FVector(0, 0, 50.0f);
	}

	cameraArm->SetWorldLocation(location);
	cameraArm->TargetArmLength = targetArmLength;
	cameraArm->SetWorldRotation(FRotator(targetPitch, targetYaw, 0.0f));
}

void UCameraManagerComponent::updateThirdPersonShooterCamera(float deltaTime) {

	if (isBot()) {
		targetArmLength = 300.0f;
		updateThirdPersonFreeCamera(deltaTime);
		return;
	}

	FVector rootLocation;
	if (GetOwner()->IsA(ACreature::StaticClass())) {
		rootLocation = ((ACreature*)GetOwner())->GetMesh()->GetComponentLocation();
		rootLocation.Z = ((ACreature*)GetOwner())->GetMesh()->GetSocketLocation("Neck").Z;
	}
	else {
		rootLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 50.0f);
	}

	bool altHanded = false;
	bool aiming = false;
	bool sprinting = false;
	ADualAxisCreature* creature = Cast< ADualAxisCreature >(GetOwner());
	if (creature) {
		altHanded = creature->altHanded;
		aiming = creature->aiming && creature->getActionComponent()->allowAim();
		sprinting = creature->sprinting && creature->inputMove.Y > 0.75f;
	}
	FRotator controlRotation = FRotator(targetPitch, targetYaw, 0.0f);

	FVector offset = FVector(150.0f,-85.0f,-15.0f);
	if (aiming) {
		offset.X *= 0.35f;
		offset.Y *= 0.65f;
	}else if (sprinting) {
		offset.X *= 0.75f;
		offset.Y *= 0.75f;
	}
	if (altHanded) {
		offset.Y *= -1.0f;
	}

	if (snapToPointEnabled) {
		snapToPointEnabled = false;

		/*FRotator snapRotation = UMathTools::getYawAndPitch(location + FRotator(targetPitch, targetYaw, 0.0f).RotateVector(offset), snapToPointLocation);
		targetYaw = snapRotation.Yaw;
		targetPitch = snapRotation.Pitch;
		syncControlRotation();*/
	}

	if (justSwitchedModes) {
		lastOffset = offset;
		lastTargetArmLength = targetArmLength;
	}

	offset = UMathTools::lerpVectorByPerc(lastOffset, offset, deltaTime * 15.0f);
	FVector finalLocation = rootLocation + controlRotation.RotateVector(offset);
	targetArmLength = FVector::Dist(rootLocation,finalLocation);

	FRotator finalRotation = UMathTools::getYawAndPitch(rootLocation,finalLocation);
	if (altHanded) {
		finalRotation.Yaw -= 10.0f;
	}
	else {
		finalRotation.Yaw += 10.0f;
	}

	cameraArm->TargetArmLength = UMathTools::lerpFloatByPerc(lastTargetArmLength, targetArmLength, deltaTime * 15.0f);

	cameraArm->SetWorldLocation(rootLocation);
	cameraArm->SetWorldRotation(finalRotation);
	camera->SetWorldRotation(controlRotation);
	lastOffset = offset;

	if (cameraArm->IsCollisionFixApplied()) {
		float dist = FVector::Dist(rootLocation, camera->GetComponentLocation());
		if (dist < 100.0f) {
			creature->hideFromOwner();
		}
		else {
			creature->showToOwner();
		}
	}
	else {
		creature->showToOwner();
	}


	float aimAssistDistance = 4000.0f;
	FVector aimAssistStart = camera->GetComponentLocation();
	FVector aimAssistEnd = aimAssistStart + camera->GetComponentRotation().RotateVector(FVector::ForwardVector * aimAssistDistance);
	TArray< FHitResult > aimAssistHits = UCollisionTools::getSphereTraceOverlaps(GetWorld(), aimAssistStart, aimAssistEnd, 100.0f, true, true);
	FVector aimDirection = aimAssistEnd - aimAssistStart;
	aimDirection.Normalize();

	FVector aimFromOwnerDirection = aimAssistEnd - GetOwner()->GetActorLocation();
	aimFromOwnerDirection.Normalize();

	aimAssistTarget = nullptr;
	aimAssistWeight = 0.0f;
	FVector aimTargetLocation;
	UTeam* team = UTeam::get(GetOwner());

	for (FHitResult& hit : aimAssistHits) {

		if (hit.GetActor() == GetOwner()) {
			continue;
		}

		ACreature* targetCreature = Cast< ACreature >(hit.GetActor());
		if (!targetCreature || targetCreature->dead) {
			continue;
		}

		if (team && !team->isHostile(UTeam::get(targetCreature))) {
			continue;
		}

		FVector currentTargetLocation = targetCreature->GetMesh()->GetBoneLocation("Spine2");

		FVector actorDirection = currentTargetLocation - camera->GetComponentLocation();
		actorDirection.Normalize();

		FVector actorFromOwnerDirection = currentTargetLocation - GetOwner()->GetActorLocation();
		actorFromOwnerDirection.Normalize();

		float rotationPerc = FVector::DotProduct(aimDirection, actorFromOwnerDirection);
		float rotationFromOwnerPerc = FVector::DotProduct(aimFromOwnerDirection, actorDirection);
		if (rotationPerc < 0.25f || rotationFromOwnerPerc < 0.25f) {
			continue;
		}

		if (!UCollisionTools::hasLineOfSight(GetWorld(), GetOwner(), targetCreature, aimAssistStart, currentTargetLocation)) {
			continue;
		}

		float distancePerc = (1.0f - (FVector::Dist(aimTargetLocation, GetOwner()->GetActorLocation()) / aimAssistDistance)) * 0.25f;
		rotationPerc = (rotationPerc + distancePerc) / 1.25f;

		if (aimAssistTarget == nullptr || rotationPerc > aimAssistWeight) {
			aimAssistTarget = targetCreature;
			aimAssistWeight = rotationPerc;
			aimTargetLocation = currentTargetLocation;
		}
	}

	if (aimAssistTarget != nullptr) {
		aimAssistWeight = UKismetMathLibrary::MultiplyMultiply_FloatFloat(FMath::Min(1.0f, aimAssistWeight), 10.0f);
		if (!aiming) {
			aimAssistWeight *= 0.1f;
		}
		else {
			aimAssistWeight *= 0.75f;
		}

		float aimAssistInputReduction = 1.0f - aimAssistWeight;

		FRotator aimAssistToTargetRotation = UMathTools::getYawAndPitch(camera->GetComponentLocation(), aimTargetLocation);
		float yawDiff = UMathTools::getAngleDiff(controlRotation.Yaw, aimAssistToTargetRotation.Yaw);
		if (yawDiff >= 0.0f) {
			aimAssistWeightXPositiveTurn = 1.0f;
			aimAssistWeightXNegativeTurn = aimAssistInputReduction;
		}
		else {
			aimAssistWeightXPositiveTurn = aimAssistInputReduction;
			aimAssistWeightXNegativeTurn = 1.0f;
		}

		float pitchDiff = UMathTools::getAngleDiff(controlRotation.Pitch, aimAssistToTargetRotation.Pitch);
		if (pitchDiff >= 0.0f) {
			aimAssistWeightYPositiveTurn = 1.0f;
			aimAssistWeightYNegativeTurn = aimAssistInputReduction;
		}
		else {
			aimAssistWeightYPositiveTurn = aimAssistInputReduction;
			aimAssistWeightYNegativeTurn = 1.0f;
		}

		handleTurnYaw(yawDiff * aimAssistWeight);
		handleTurnPitch(pitchDiff * aimAssistWeight);

		//UDebug::tick("Aim Assist Targeting " + aimAssistTarget->GetName() + " with weight " + FString::SanitizeFloat(aimAssistWeight) + " yaw diff: " + FString::SanitizeFloat(yawDiff));
	}

}

void UCameraManagerComponent::updateTopDownCamera(float deltaTime) {
	cameraArm->SetRelativeLocation(FVector(0, 0, 50.0f));
	cameraArm->TargetArmLength = targetArmLength;
	cameraArm->SetWorldRotation(FRotator(targetPitch, targetYaw, 0.0f));
}

void UCameraManagerComponent::updateCharacterLimitedCamera(float deltaTime) {

	ACreature* creature = Cast<ACreature>(focusedOnActor);
	if (creature == nullptr) {
		creature = Cast<ACreature>(GetOwner());
	}
	if (creature == nullptr) {
		return;
	}

	FVector cameraLocation = creature->GetActorLocation();
	cameraLocation.Z = creature->GetMesh()->GetBoneLocation(mode == ECameraMode::CHARACTER_FACE_LIMITED ? "Head" : "Hips").Z;

	cameraArm->SetWorldLocation(cameraLocation);
	cameraArm->TargetArmLength = (mode == ECameraMode::CHARACTER_FACE_LIMITED ? 125.0f : 400.0f);
	cameraArm->SetWorldRotation(FRotator(0.0f, creature->bodyYaw - 90.0f, 0.0f));
}


void UCameraManagerComponent::updateCharacterDeadCamera(float deltaTime) {

	ACreature* creature = Cast<ACreature>(focusedOnActor);
	if (creature == nullptr) {
		creature = Cast<ACreature>(GetOwner());
	}
	if (creature == nullptr) {
		return;
	}

	cameraArm->SetWorldLocation(lastLocation);
	cameraArm->TargetArmLength = UMathTools::lerpFloatByAmount(cameraArm->TargetArmLength, 400.0f, deltaTime*500.0f);
	cameraArm->SetWorldRotation(lastRotation);

	FRotator rotation = UMathTools::getYawAndPitch(camera->GetComponentLocation(), creature->GetMesh()->GetSocketLocation("Head"));
	rotation = UMathTools::lerpRotatorByAmount(camera->GetComponentRotation(),rotation, deltaTime * 20.0f);
	camera->SetWorldRotation(rotation);

}

void UCameraManagerComponent::updateRTSCamera(float deltaTime) {
	cameraArm->SetRelativeLocation(FVector(0, 0, 100.0f));
	cameraArm->TargetArmLength = targetArmLength;
	targetPitch = FMath::Clamp(targetPitch, -65, -15);
	cameraArm->SetWorldRotation(FRotator(targetPitch, targetYaw, 0.0f));
}

void UCameraManagerComponent::bindInput(bool uiFocused) {
	UInputComponent* inputComponent = Cast< UInputComponent >(GetOwner()->GetComponentByClass(UInputComponent::StaticClass()));
	if (!inputComponent) {
		return;
	}
	if (!uiFocused) {
		inputComponent->BindAxis("LookX", this, &UCameraManagerComponent::handleTurnYaw);
		inputComponent->BindAxis("LookY", this, &UCameraManagerComponent::handleTurnPitch);
		inputComponent->BindAction("RotateCamera", IE_Pressed, this, &UCameraManagerComponent::pressDragRotation);
		inputComponent->BindAction("RotateCamera", IE_Released, this, &UCameraManagerComponent::releaseDragRotation);
		inputComponent->BindAction("ZoomIn", IE_Pressed, this, &UCameraManagerComponent::pressZoomIn);
		inputComponent->BindAction("ZoomOut", IE_Pressed, this, &UCameraManagerComponent::pressZoomOut);
	}
}

void UCameraManagerComponent::clearInput() {
	draggingRotation = false;
}

void UCameraManagerComponent::handleTurnYaw(float amount) {
	if (amount >= -0.01f && amount <= 0.01f) {
		return;
	}
	if (canTurnYaw && (!showCursor || draggingRotation)) {

		if (aimAssistTarget) {
			if (amount >= 0.0f) {
				amount *= aimAssistWeightXPositiveTurn;
			}
			else {
				amount *= aimAssistWeightXNegativeTurn;
			}
		}

		targetYaw += amount * turnSpeed;
		needsSyncOwner = true;
	}
}

void UCameraManagerComponent::handleTurnPitch(float amount) {
	if (amount >= -0.01f && amount <= 0.01f) {
		return;
	}
	if (canTurnPitch && (!showCursor || draggingRotation)) {

		if (aimAssistTarget) {
			if (amount >= 0.0f) {
				amount *= aimAssistWeightYPositiveTurn;
			}
			else {
				amount *= aimAssistWeightYNegativeTurn;
			}
		}

		targetPitch += amount * turnSpeed;
		targetPitch = UMathTools::clampPitch(targetPitch, 60.0f);
		needsSyncOwner = true;
	}
}

void UCameraManagerComponent::handleTurn(FVector2D amount) {
	handleTurnYaw(amount.X);
	handleTurnPitch(amount.Y);
}

void UCameraManagerComponent::syncControlRotation() {
	if (GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* pawn = Cast< ACreature >(GetOwner());
		if (!pawn->isBot) {
			pawn->controlRotation = FRotator(targetPitch, targetYaw, 0.0f);
		}
	}
}

bool UCameraManagerComponent::isBot() {
	if (GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* pawn = Cast< ACreature >(GetOwner());
		return pawn->isBot;
	}
	return false;
}


void UCameraManagerComponent::pressDragRotation() {
	draggingRotation = true;
}

void UCameraManagerComponent::releaseDragRotation() {
	draggingRotation = false;
}

void UCameraManagerComponent::pressZoomIn() {
	zoomIn();
}

void UCameraManagerComponent::pressZoomOut() {
	zoomOut();
}

FVector UCameraManagerComponent::getCameraLocation() {
	return camera ? camera->GetComponentLocation() : FVector::ZeroVector;
}

FRotator UCameraManagerComponent::getCameraRotation() {
	return camera ? camera->GetComponentRotation() : FRotator::ZeroRotator;
}

void UCameraManagerComponent::snapToPoint(FVector location) {
	snapToPointEnabled = true;
	snapToPointLocation = location;
}

void UCameraManagerComponent::zoomIn() {
	if (canZoom) {
		targetArmLength = FMath::Max(minTargetArmLength, targetArmLength - targetArmLengthZoomStep);
	}
}

void UCameraManagerComponent::zoomOut() {
	if (canZoom) {
		targetArmLength = FMath::Min(maxTargetArmLength, targetArmLength + targetArmLengthZoomStep);
	}
}


UCameraManagerComponent* UCameraManagerComponent::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}
	return Cast< UCameraManagerComponent >(actor->GetComponentByClass(UCameraManagerComponent::StaticClass()));
}

void UCameraManagerComponent::remove(AActor* actor) {
	UCameraManagerComponent* component = get(actor);
	if (component) {
		component->DestroyComponent();
	}
}

UCameraManagerComponent* UCameraManagerComponent::add(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	UCameraManagerComponent* cameraManagerComponent = NewObject< UCameraManagerComponent >(actor, UCameraManagerComponent::StaticClass());
	cameraManagerComponent->RegisterComponent();
	return cameraManagerComponent;
}