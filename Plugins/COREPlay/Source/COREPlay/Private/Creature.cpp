#include "Creature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"


#include "ComplexPlayerController.h"
#include "CameraManagerComponent.h"
#include "StatsComponent.h"
#include "ComplexGameState.h"
#include "ActionComponent.h"
#include "BodyComponent.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "VesselComponent.h"
#include "FaceComponent.h"
#include "MindComponent.h"
#include "ComplexHUD.h"
#include "FXList.h"
#include "Team.h"
#include "WorldWidget.h"
#include "WorldDecal.h"
#include "CollisionTools.h"
#include "MathTools.h"
#include "Debug.h"

int ACreature::lastID = 0;
TArray< ACreature* > ACreature::instances;
float ACreature::replicationSmoothRate = 10.0f;



ACreature::ACreature() {
	PrimaryActorTick.bCanEverTick = true;
	//AIControllerClass = AB3DAIController::StaticClass();
	this->bReplicates = true;
}

void ACreature::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ACreature, ready, COND_InitialOnly);
	DOREPLIFETIME(ACreature, name);
	DOREPLIFETIME(ACreature, id);
	DOREPLIFETIME(ACreature, spec);
	DOREPLIFETIME(ACreature, team);
	DOREPLIFETIME(ACreature, isBot);
	DOREPLIFETIME(ACreature, despawnTime);
}


void ACreature::BeginPlay() {
	Super::BeginPlay();
	if (!instances.Contains(this)) {
		instances.Add(this);
	}
	clearInput();
	resetControlRotation();
}

void ACreature::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	deSpawn();
	Super::EndPlay(EndPlayReason);
}

ACreature* ACreature::getByID(int id) {
	for (ACreature* creature : instances) {
		if (creature != nullptr && creature->id == id) {
			return creature;
		}
	}
	return nullptr;
}

void ACreature::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!spawned) {
		if (ready && spawn()) {
			spawned = true;
			if (dead) {
				kill();
			}
		}
	}
	else {

		isBot = UMindComponent::get(this) != nullptr;
		if (!isBot && spec && spec->spawnMindIfUncontrolled && (!Controller || !Controller->IsA(APlayerController::StaticClass()))) {
			isBot = (UMindComponent::add(this) != nullptr);
		}

		if (dead) {
			if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
				despawnTime -= DeltaTime;
				if (despawnTime <= 0.0f) {

					AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
					if (gameState) {
						gameState->despawnPawn(this);
					}
					Destroy();
					return;
				}
			}
		}
		else {
			processInput(DeltaTime);
			if (GetMesh()->GetAttachParent() == GetCapsuleComponent()) {
				GetMesh()->SetWorldRotation(FRotator(0.0f, bodyYaw, 0.0f));
			}
		}

		if (GetWorld()->IsNetMode(ENetMode::NM_Client)) {
			if (IsLocallyControlled()) {
				if (isBot) {
					if (playerInputPressed) {
						requestPayload(FPayload(FPayload::TYPE_REQUEST_PLAYER_CONTROL));
					}
				}
				else {
					inputReplicationTimer -= DeltaTime;
					if (inputReplicationTimer <= 0.0f) {
						inputReplicationTimer = 0.1f;
						sendInputToServer(DeltaTime);
					}
				}
			}
			else {
				controlRotation = UMathTools::lerpRotatorByPerc(controlRotation, netControlRotation, DeltaTime * replicationSmoothRate);
				if (GetController()) {
					GetController()->SetControlRotation(netControlRotation);
				}
			}
		}
		else {
			if (isBot && playerInputPressed) {
				requestPayload(FPayload(FPayload::TYPE_REQUEST_PLAYER_CONTROL));
			}
			inputReplicationTimer -= DeltaTime;
			if (inputReplicationTimer <= 0.0f) {
				inputReplicationTimer = 0.1f;
				sendInputToClients(DeltaTime);
			}

			if(!IsLocallyControlled()) {
				controlRotation = UMathTools::lerpRotatorByPerc(controlRotation, netControlRotation, DeltaTime * replicationSmoothRate);
				if (GetController()) {
					GetController()->SetControlRotation(netControlRotation);
				}
			}

		}
		playerInputPressed = false;
	}
}

void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	bool uiFocused = false;
	if (Controller) {
		AComplexPlayerController* playerController = Cast< AComplexPlayerController >(Controller);
		if (playerController) {
			uiFocused = playerController->uiFocused;
		}
	}
	rebindInput(uiFocused);
}

void ACreature::showToOwner() {
	if (hiddenFromOwner) {
		hiddenFromOwner = false;

		UBodyComponent* body = UBodyComponent::get(this);
		if (body) {
			body->refreshHiddenFromOwner();
		}
	}
}

void ACreature::hideFromOwner() {
	if (!hiddenFromOwner) {
		hiddenFromOwner = true;

		UBodyComponent* body = UBodyComponent::get(this);
		if (body) {
			body->refreshHiddenFromOwner();
		}
	}
}

UActionComponent* ACreature::getActionComponent() {
	if (actionComponent == nullptr) {
		actionComponent = Cast<UActionComponent>(GetComponentByClass(UActionComponent::StaticClass()));
		if (actionComponent == nullptr) {
			actionComponent = NewObject<UActionComponent>(this);
			actionComponent->RegisterComponent();
		}
	}
	return actionComponent;
}

float ACreature::getCapsuleRadius() {
	return GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float ACreature::getCapsuleHalfHeight() {
	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

float ACreature::getMoveRate() {
	if (isFalling()) {
		return 0.5f;
	}
	return 1.0f;
}

float ACreature::getMovePerc() {
	return GetVelocity().Size2D() / GetMovementComponent()->GetMaxSpeed();
}

float ACreature::getInputYaw() {
	if (inputMove.Size() > 0.01f) {
		return UMathTools::getAngle(FVector2D(-inputMove.X, inputMove.Y).GetRotated(controlRotation.Yaw)) - 90.0f;
	}
	return bodyYaw;
}

void ACreature::faceMovement(float deltaTime) {
	float movementYaw = getInputYaw();
	faceYaw(movementYaw, deltaTime * 300.0f);
}

void ACreature::faceControl() {
	bodyYaw = controlRotation.Yaw - 90.0f;
}

void ACreature::faceTarget(AActor* target, float deltaTime) {
	if (!target) {
		return;
	}
	faceYaw(UMathTools::getYaw(GetActorLocation(), target->GetActorLocation()) - 90.0f, deltaTime*300.0f);
}

void ACreature::faceYaw(float yaw, float deltaTime) {
	bodyYaw = UMathTools::lerpAngleByDegrees(bodyYaw, yaw, deltaTime);
}

void ACreature::snapToBodyYaw() {
	GetMesh()->SetWorldRotation(FRotator(0.0f, bodyYaw, 0.0f));
}

void ACreature::snapMeshPosition() {
	FVector meshLocation = GetMesh()->GetRelativeLocation();
	FVector actorLocation = GetActorLocation();
	GetMesh()->SetWorldLocation(actorLocation + meshLocation );
}

UCreatureAnimInstance* ACreature::getAnimInstance() {
	return Cast< UCreatureAnimInstance >(GetMesh()->GetAnimInstance());
}

UAnimSequence* ACreature::getAnimSequence(FString key) {
	UCreatureAnimInstance* creatureAnimInstance = getAnimInstance();
	if (creatureAnimInstance) {
		return creatureAnimInstance->getSequence(key);
	}
	return nullptr;
}

UAnimMontage* ACreature::playAnimSequence(FString key, float rate, float startTime, float blendIn, float blendOut, int loops, FString slot) {
	UAnimSequence* animSequence = getAnimSequence(key);
	if(animSequence == nullptr) {
		UDebug::error("Failed to play creature animation! Sequence '" + key + "' could not be found.");
		return nullptr;
	}
	return playAnimSequence(animSequence, rate, startTime, blendIn, blendOut, loops, slot);
}


UAnimMontage* ACreature::playAnimSequence(UAnimSequence* animSequence, float rate, float startTime, float blendIn, float blendOut, int loops, FString slot) {
	UCreatureAnimInstance* creatureAnimInstance = getAnimInstance();
	if (creatureAnimInstance && animSequence) {
		float animLength = animSequence->GetPlayLength();
		UAnimMontage* montage = creatureAnimInstance->PlaySlotAnimationAsDynamicMontage(animSequence, FName(*slot), (blendIn * animLength) / rate, (blendOut * animLength) / rate, rate, FMath::Max(1, loops), -1.0f, rate < 0.0f ? 1.0f - startTime : startTime);
		if (loops <= 0) {
			creatureAnimInstance->Montage_SetNextSection("Default", "Default", montage);
		}
		creatureAnimInstance->lastAnimations.Add(slot, montage);
		return montage;
	}
	else {
		UDebug::error("Failed to play creature animation!");
	}
	return nullptr;
}

void ACreature::fadeOutCurrentAnim(UAnimMontage* montage, float fadeOut, FString slot) {
	UCreatureAnimInstance* creatureAnimInstance = getAnimInstance();
	if (creatureAnimInstance) {

		if (montage) {
			float playRate = creatureAnimInstance->Montage_GetPlayRate(montage);
			if (playRate == 0.0f) {
				playRate = 1.0f;
			}
			fadeOut = (fadeOut * montage->GetPlayLength()) / playRate;
		}

		creatureAnimInstance->StopSlotAnimation(fadeOut, FName(*slot));
	}
}

void ACreature::stopLoopingAnimSequence(UAnimMontage* montage) {
	UCreatureAnimInstance* creatureAnimInstance = getAnimInstance();
	if (creatureAnimInstance && montage) {
		creatureAnimInstance->Montage_SetNextSection("Default", "NONE", montage);
	}
}

void ACreature::stopSlotAnimation(float fadeOut, FString slot) {
	UCreatureAnimInstance* creatureAnimInstance = getAnimInstance();
	if (creatureAnimInstance) {
		creatureAnimInstance->StopSlotAnimation(fadeOut, FName(*slot));
	}
}

void ACreature::handlePayload(FPayload payload) {
	FString type = payload.getString("type");

	if (FPayload::TYPE_JUMP.Equals(type)) {
		tryJumpUp();
	}
	else if (FPayload::TYPE_LAUNCH.Equals(type)) {
		bool inBodyDirection = payload.getBool("inBodyDirection");
		FVector direction = payload.getVector("direction",FVector::ZeroVector);

		if (inBodyDirection) {
			launch(FRotator(0.0f, bodyYaw, 0.0f).RotateVector(direction));
		}
		else {
			launch(direction);
		}
	}
	else if (FPayload::TYPE_PLAY_ANIM.Equals(type)) {
		playAnimSequence(payload.getString("key"), payload.getNumeric("rate"), payload.getNumeric("startTime"), payload.getNumeric("blendIn"), payload.getNumeric("blendOut"), payload.getNumeric("loops"), payload.getString("slot"));
	}
	else if (FPayload::TYPE_BODY_CHANGE.Equals(type)) {
		UBodyComponent* body = UBodyComponent::get(this);
		if (body) {
			body->processPayload(payload);
		}
	}
	else if (FPayload::TYPE_REQUEST_PLAYER_CONTROL.Equals(type)) {
		UMindComponent::remove(this);
		isBot = false;
	}
	else {
		getActionComponent()->queuePayload(payload);
	}
}


void ACreature::initID() {
	if (id == -1) {
		id = lastID;
		lastID++;
	}
}

bool ACreature::spawn() {

	if (GetWorld() && !GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		initID();
		if (spec) {
			UInventoryComponent* inventory = UInventoryComponent::get(this);
			UEquipmentComponent* equipment = UEquipmentComponent::get(this);

			if (inventory) {
				inventory->applyItemSet(spec->spawnItems);
			}

			equipment->refresh();
		}
	}

	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	refreshMaterials();

	//makeBot();
	return true;
}

void ACreature::clearInput() {
	inputMove = FVector2D::ZeroVector;
}


void ACreature::processInput(float deltaTime) {

	if (IsLocallyControlled()) {
		UCharacterMovementComponent* movementComponent = GetCharacterMovement();

		if (inputMove.Size() > 0.01f && getActionComponent()->allowMove()) {
			FRotator movementRotation(0.0f, controlRotation.Yaw, 0.0f);

			FVector2D finalInputMove = inputMove;
			if (finalInputMove.Size() > 1.0f) {
				finalInputMove.Normalize();
			}

			movementComponent->AddInputVector(movementRotation.RotateVector(FVector(finalInputMove.Y, finalInputMove.X, 0.0f)) * getMoveRate());

			if (!aimLocked) {
				faceMovement(deltaTime);
			}
		}
		if (aimLocked && getActionComponent()->allowTurn()) {
			faceControl();
		}
	}
	else {
		if (inputMove.Size() > 0.01f && getActionComponent()->allowMove() && !aimLocked) {
			faceMovement(deltaTime);
		}
		if (aimLocked && getActionComponent()->allowTurn()) {
			faceControl();
		}
	}
}

void ACreature::rebindInput() {
	DestroyPlayerInputComponent();
	CreatePlayerInputComponent();
}

void ACreature::rebindInput(bool uiFocused) {
	UInputComponent* inputComponent = Cast< UInputComponent >(GetComponentByClass(UInputComponent::StaticClass()));
	if (inputComponent == nullptr) {
		return;
	}
	inputComponent->ClearActionBindings();
	inputComponent->ClearBindingValues();
	inputComponent->AxisBindings.Empty();
	clearInput();
	bindInput(inputComponent, uiFocused);

	UCameraManagerComponent* cameraManager = UCameraManagerComponent::get(this);
	if (cameraManager) {
		cameraManager->clearInput();
		cameraManager->bindInput(uiFocused);
	}
}

void ACreature::bindInput(UInputComponent* inputComponent, bool uiFocused) {
	if (!uiFocused) {
		inputComponent->BindAxis("MoveX", this, &ACreature::pressInputMoveX);
		inputComponent->BindAxis("MoveY", this, &ACreature::pressInputMoveY);
		inputComponent->BindAction("Jump", IE_Pressed, this, &ACreature::pressJump);
	}
}


void ACreature::resetControlRotation() {
	controlRotation = FRotator(0.0f, bodyYaw + 90.0f, 0.0f);
}

void ACreature::deSpawn() {

}

void ACreature::kill() {
	if (dead) {
		return;
	}

	dead = true;
	despawnTime = spec ? spec->despawnTime : 10.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	GetMovementComponent()->Deactivate();

	UCameraManagerComponent* cameraManager = UCameraManagerComponent::get(this);
	if (cameraManager) {
		cameraManager->switchMode(ECameraMode::CHARACTER_DEAD);
	}

	UFaceComponent* face = UFaceComponent::get(this);
	if (face) {
		face->applyDeadExpression();
	}

	getActionComponent()->stopAllActions();

	if (!isRagdolling) {
		startRagdoll();
	}
}

void ACreature::processKill(FString killerName, FColor killerColor, FString iconKey, FString effectKey) {
	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (gameState) {
		gameState->scoreDeath(this, killerName, killerColor, iconKey);
	}
	FDamageDetails damage;
	damage.effectKey = effectKey;
	sendKill(damage);
}

void ACreature::detachMesh(bool updateAnimsImmediately) {
	detachMesh(GetMesh()->GetComponentLocation(), GetMesh()->GetComponentRotation(), updateAnimsImmediately);
}

void ACreature::detachMesh(FVector location, FRotator rotation, bool updateAnimsImmediately) {
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->SetWorldLocation(location);
	GetMesh()->SetWorldRotation(rotation);
	if (updateAnimsImmediately) {
		updateAnimationsImmediately();
	}
}

void ACreature::reattachMesh(bool updateAnimsImmediately) {
	GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetWorldRotation(FRotator(0.0f, bodyYaw, 0.0f));
	recenterMesh();
	if (updateAnimsImmediately) {
		updateAnimationsImmediately();
	}
}

void ACreature::updateAnimationsImmediately() {
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		animInstance->updateAnimationImmediately();
	}
}

void ACreature::recenterMesh() {
	GetMesh()->SetRelativeLocation(FVector(0, 0, -getCapsuleHalfHeight() - 3.5f));
}

void ACreature::lookAt(FVector location, bool turnHead) {
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		if (turnHead) {
			animInstance->setLookAt(location, 1.0f);
		}
		else {
			animInstance->clearLookAt(false);
		}
	}

	UFaceComponent* face = UFaceComponent::get(this);
	if (face) {
		face->setEyeTarget(nullptr, location);
	}
}

void ACreature::clearLookAt() {
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		animInstance->clearLookAt(false);
	}

	UFaceComponent* face = UFaceComponent::get(this);
	if (face) {
		face->clearEyeTarget();
	}
}


bool ACreature::isFalling() {
	return GetMovementComponent()->IsFalling();
}

bool ACreature::launch(FVector launchAmount) {
	LaunchCharacter(launchAmount, false, false);
	return true;
}

bool ACreature::tryJumpUp() {
	if (isFalling()) {
		return false;
	}
	Jump();
	return true;
}

bool ACreature::canStartJump() {
	return !isFalling() && getActionComponent()->allowJump() && !dead;
}

bool ACreature::startJump() {
	requestPayload(FPayload::playAnimSequence("Jump"));
	return true;
}

void ACreature::startRagdoll() {
	GetMesh()->SetAllBodiesBelowSimulatePhysics("Hips", true, true);
	if (Controller) {
		Controller->StopMovement();
	}
	isRagdolling = true;
}

void ACreature::refreshMaterials() {

	UBodyComponent* body = UBodyComponent::get(this);
	if (!body) {
		return;
	}

	UStatsComponent* stats = UStatsComponent::get(this);
	if (stats && stats->hasStat("HEALTH")) {
		body->setScalarParameter("DamagePerc", 1.0f - stats->getStatPerc("HEALTH"), false);
	}

	if (team == nullptr) {
		team = UTeam::get(this);
	}
	if (team) {
		body->setColorParameter("TeamColor", team->color, false);
	}
}

int ACreature::getActualDamageAmount(FTargetDetail& attacker, FDamageDetail& damageDetail) {
	UTeam* attackerTeam = UTeam::get(GetWorld(), attacker);
	if (attackerTeam && !attackerTeam->isHostile(team)) {
		return damageDetail.amount * attackerTeam->friendlyFireDamage;
	}
	return damageDetail.amount;
}

float ACreature::getActualStagger(FDamageDetails& damage) {
	float stagger = damage.stagger;

	UTeam* attackerTeam = UTeam::get(GetWorld(), damage.attacker);
	if (attackerTeam && !attackerTeam->isHostile(team)) {
		return stagger * attackerTeam->friendlyFireDamage;
	}

	if(spec){
		stagger -= spec->staggerResistance;
	}
	return stagger;
}

float ACreature::getActualForce(FDamageDetails& damage) {
	float force = damage.force;

	UTeam* attackerTeam = UTeam::get(GetWorld(), damage.attacker);
	if (attackerTeam && !attackerTeam->isHostile(team)) {
		return force * attackerTeam->friendlyFireDamage;
	}

	if (spec) {
		force -= spec->forceResistance;
	}
	return force;
}

void ACreature::processServerDamage(FDamageDetails damage) {

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());

	bool killingBlow = false;
	if (!dead) {
		int totalDamage = 0;
		for (FDamageDetail& detail : damage.damages) {
			detail.amount = getActualDamageAmount(damage.attacker, detail);
			totalDamage += detail.amount;
		}

		if (damage.attacker.type == ETargetClass::CREATURE) {
			UAISense_Damage::ReportDamageEvent(GetWorld(), this, damage.attacker.getCreature(GetWorld()),0.0f, GetActorLocation(), damage.victim.location);
		}

		UStatsComponent* stats = UStatsComponent::get(this);
		if (stats) {
			if (stats->hasStat("HEALTH")) {
				stats->addStatValue("HEALTH", -totalDamage);
				stats->setStatRegenTime("HEALTH", 10.0f);
				if (stats->getStatValue("HEALTH") <= 0) {
					killingBlow = true;
				}
			}
		}

		if (!killingBlow && damage.stagger > 0.01f) {
			float stagger = getActualStagger(damage);
			if (stagger >= 0.5f) {
				sendStagger(damage);
			}
		}

		if (gameState) {
			gameState->scoreDamage(damage, totalDamage);
			if (killingBlow) {
				gameState->scoreKill(damage, totalDamage);
			}
		}
	}
	sendDamage(damage, killingBlow);
}

void ACreature::processClientDamage(FDamageDetails damage, bool killingBlow) {
	if (spec) {
		FString defaultEffectKey = "Default";
		FString effectKey = "";

		if (damage.damages.Num() > 0) {
			for (int d = 0; d < damage.damages.Num(); d++) {
				if (damage.damages[d].type == EDamageClass::SLASH) {
					effectKey = "Slash";
				}
				else if (damage.damages[d].type == EDamageClass::PIERCE) {
					effectKey = "Pierce";
				}
			}
		}

		if (spec->hitEffects.Contains(effectKey)) {
			spec->hitEffects[effectKey]->execute(this, damage.victim.location, UMathTools::getYaw(damage.victim.location, damage.attacker.location), FVector(1.0f));
		}
		else if (spec->hitEffects.Contains(defaultEffectKey)) {
			spec->hitEffects[defaultEffectKey]->execute(this, damage.victim.location, UMathTools::getYaw(damage.victim.location, damage.attacker.location), FVector(1.0f));
		}

		if (spec->hitBloodDecals.Contains(effectKey)) {
			FHitResult hit = UCollisionTools::getRayHit(GetWorld(), damage.victim.location, damage.victim.location + FVector(0.0f, 0.0f, -getCapsuleHalfHeight() * 2.0f), ECollisionChannel::ECC_Pawn, this);
			if (hit.bBlockingHit) {
				AWorldDecal::spawnWorldDecal(GetWorld(), hit.Location + UMathTools::randomVectorFromRange(30.0f, 30.0f, 0.0f), FRotator::ZeroRotator, spec->hitBloodDecals[effectKey]);
			}
		}
		else if (spec->hitBloodDecals.Contains(defaultEffectKey)) {
			FHitResult hit = UCollisionTools::getRayHit(GetWorld(), damage.victim.location, damage.victim.location + FVector(0.0f, 0.0f, -getCapsuleHalfHeight() * 2.0f), ECollisionChannel::ECC_Pawn, this);
			if (hit.bBlockingHit) {
				AWorldDecal::spawnWorldDecal(GetWorld(), hit.Location + UMathTools::randomVectorFromRange(30.0f, 30.0f, 0.0f), FRotator::ZeroRotator, spec->hitBloodDecals[defaultEffectKey]);
			}
		}
	}

	if (!dead && !killingBlow && damage.force > 0.01f) {
		float force = getActualForce(damage);
		if (force >= 1.0f) {

			if (damage.forceFaceAttackerAim) {
				FVector direction = controlRotation.RotateVector(FVector(-1.0f, 0.0f, 0.0f));
				if (damage.forceOnlyUsesXY) {
					direction.Z = 0.0f;
				}
				direction.Normalize();

				launch(direction * force);
			}
			else {
				FVector direction = GetActorLocation() - damage.attacker.location;
				if (damage.forceOnlyUsesXY) {
					direction.Z = 0.0f;
				}
				direction.Normalize();

				launch(direction * force);
			}
		}
	}

	UFXList* fxList = UResourceCache::getFXList(damage.effectKey);
	if (fxList) {
		fxList->execute(this, GetActorLocation(), bodyYaw, FVector(1, 1, 1));
	}

	if (killingBlow && !dead) {
		kill();
	}
	else if (!dead) {
		UFaceComponent* face = UFaceComponent::get(this);
		if (face) {
			face->applyPainExpression();
		}
		UCreatureAnimInstance* animInstance = getAnimInstance();
		if (animInstance ) {
			playAnimSequence(getHitReactionAnim(damage), 1.0f, 0.0f, 0.1f, 0.1f, 1, "Hit");
		}
	}

	if (dead && isRagdolling) {
		FName boneName = damage.victim.getHitBoneName();
		if (boneName != "") {
			FVector direction = damage.victim.location - damage.attacker.location;
			direction.Normalize();
			FVector finalImpulse = direction * damage.force * 25.0f;
			if (finalImpulse.Size() > 0.01f) {
				GetMesh()->AddImpulse(finalImpulse, boneName, true);
			}
		}
	}

	refreshMaterials();
}

void ACreature::sendDamage_Implementation(FDamageDetails damage, bool killingBlow) {
	processClientDamage(damage, killingBlow);
}

FString ACreature::getHitReactionAnim(FDamageDetails& damage) {
	return "HitReaction";
}


void ACreature::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	onPossessed(NewController);
	showToOwner();
}

void ACreature::onPossessed_Implementation(AController* NewController) {
	if (NewController && NewController->IsA(APlayerController::StaticClass()) && NewController->IsLocalController() && UCameraManagerComponent::get(this) == nullptr) {
		UCameraManagerComponent::add(this);
	}
	AComplexHUD::Possess(this);
}

void ACreature::UnPossessed() {
	onUnpossessed();
	showToOwner();
	Super::UnPossessed();
}

void ACreature::onUnpossessed_Implementation() {
	UCameraManagerComponent::remove(this);
	//AComplexHUD::UnPossess(this);
}

void ACreature::updateInputPayload() {
	clientToServerPayload.setNumeric("inputMove.X", inputMove.X);
	clientToServerPayload.setNumeric("inputMove.Y", inputMove.Y);
	clientToServerPayload.setBool("playerInputPressed", playerInputPressed);

	UMindComponent* mind = UMindComponent::get(this);
	if (mind) {
		clientToServerPayload.setNumeric("mindControlRotation.Pitch", mind->targetControlRotation.Pitch);
		clientToServerPayload.setNumeric("mindControlRotation.Yaw", mind->targetControlRotation.Yaw);
	}
	else {
		clientToServerPayload.setNumeric("controlRotation.Pitch", controlRotation.Pitch);
		clientToServerPayload.setNumeric("controlRotation.Yaw", controlRotation.Yaw);
	}
}

void ACreature::sendInputToServer(float deltaTime) {
	updateInputPayload();
	requestClientInput(clientToServerPayload);
}

void ACreature::sendInputToClients(float deltaTime) {
	updateInputPayload();
	sendClientInput(clientToServerPayload);
}

void ACreature::requestClientInput_Implementation(FPayload payload) {
	receiveClientInput(payload);
}

void ACreature::sendClientInput_Implementation(FPayload payload) {
	if (IsLocallyControlled() && !isBot) {
		return;
	}
	receiveClientInput(payload);
}

void ACreature::receiveClientInput(FPayload payload) {
	inputMove = FVector2D(payload.getNumeric("inputMove.X"), payload.getNumeric("inputMove.Y"));
	playerInputPressed = payload.getBool("playerInputPressed");

	UPlayerRecord* player = UPlayerRecord::get(this);
	if (player && playerInputPressed) {
		player->lastInputTimer = 0.0f;
	}

	UMindComponent* mind = UMindComponent::get(this);
	if (mind && isBot) {
		mind->targetControlRotation = FRotator(payload.getNumeric("mindControlRotation.Pitch"), payload.getNumeric("mindControlRotation.Yaw"), 0.0f);
	}
	netControlRotation = FRotator(payload.getNumeric("controlRotation.Pitch"), payload.getNumeric("controlRotation.Yaw"), 0.0f);

}

void ACreature::requestPayload_Implementation(FPayload payload) {
	sendPayload(payload);
}

void ACreature::sendKill_Implementation(FDamageDetails damage) {
	kill();
	FName boneName = damage.victim.getHitBoneName();
	if (boneName != "") {
		FVector direction = damage.victim.location - damage.attacker.location;
		direction.Normalize();
		GetMesh()->AddImpulse(direction * damage.force * 35.0f, boneName, true);
	}

	UFXList* fxList = UResourceCache::getFXList(damage.effectKey);
	if (fxList) {
		fxList->execute(this, GetActorLocation(), bodyYaw, FVector(1, 1, 1));
	}

}

void ACreature::sendStagger(FDamageDetails damage) {
	
	FVector attackerLocation = damage.attacker.location;
	FVector currentLocation = GetActorLocation();

	float yaw = UMathTools::getYaw(currentLocation, attackerLocation);
	float angleDiff = UMathTools::getSafeAngle(UMathTools::getAngleDiff(yaw, bodyYaw));
	if (angleDiff >= 315.0f || angleDiff <= 45.0f) {
		sendPayload(FPayload::startAction("StumbleBackward"));
	}
	else if (angleDiff >= 45.0f && angleDiff <= 135.0f) {
		sendPayload(FPayload::startAction("StumbleRight"));
	}
	else if (angleDiff >= 135 && angleDiff <= 225.0f) {
		sendPayload(FPayload::startAction("StumbleForward"));
	}
	else {
		sendPayload(FPayload::startAction("StumbleLeft"));
	}
}


void ACreature::sendPayload_Implementation(FPayload payload) {
	handlePayload(payload);
}


void ACreature::pressInputMoveX(float value) {
	if (value < -0.01 || value > 0.01) {
		playerInputPressed = true;
	}
	if (!isBot) {
		inputMove.X = value;
	}
}

void ACreature::pressInputMoveY(float value) {
	if (value < -0.01 || value > 0.01) {
		playerInputPressed = true;
	}
	if (!isBot) {
		inputMove.Y = value;
	}
}

void ACreature::pressJump() {
	playerInputPressed = true;
	if (!isBot && canStartJump()) {
		startJump();
	}
}

void ACreature::equipPrimaryWeapon(FString itemKey) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(this);
	if (equipment) {
		equipment->setEquippedItem("PrimaryWeapon", itemKey, -1);
		equipment->refresh();
	}
	getActionComponent()->startBaseAction("DrawWeapon");
}


void ACreature::makeBot() {
	if (!isBot) {
		clearInput();
	}
	isBot = true;
	if (spec != nullptr && spec->mindComponentClass != nullptr && UMindComponent::get(this) == nullptr) {
		UMindComponent::add(this);
	}
	if (Controller == nullptr) {
		SpawnDefaultController();
	}
}

void ACreature::makePlayerControlled() {
	if (isBot) {
		clearInput();
	}
	UMindComponent::remove(this);
}


void ACreature::writeXML(FXMLBuilderNode& rootNode) {
	UBodyComponent* body = UBodyComponent::get(this);
	if (body) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		body->writeXML(componentNode);
	}

	UStatsComponent* stats = UStatsComponent::get(this);
	if (stats) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		stats->writeXML(componentNode);
	}

	UVesselComponent* vessel = UVesselComponent::get(this);
	if (vessel) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		vessel->writeXML(componentNode);
	}

	UActionComponent* action = UActionComponent::get(this);
	if (action) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		action->writeXML(componentNode);
	}

	UMindComponent* mind = UMindComponent::get(this);
	if (mind) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		mind->writeXML(componentNode);
	}
}

void ACreature::readXML(FXmlNode* rootNode) {
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("COMPONENT")) {
			FString componentType = UXMLTools::getAttributeString(childNode, "type");
			if (componentType.Equals("Body")) {
				UBodyComponent* body = UBodyComponent::get(this);
				if (body) {
					body->readXML(childNode);
				}
			}
			else if (componentType.Equals("Stats")) {
				UStatsComponent* stats = UStatsComponent::get(this);
				if (stats) {
					stats->readXML(childNode);
				}
			}
			else if (componentType.Equals("Vessel")) {
				UVesselComponent* vessel = UVesselComponent::get(this);
				if (vessel) {
					vessel->readXML(childNode);
				}
			}
			else if (componentType.Equals("Action")) {
				UActionComponent* action = UActionComponent::get(this);
				if (action) {
					action->readXML(childNode);
				}
			}
			else if (componentType.Equals("Mind")) {
				UMindComponent* mind = UMindComponent::get(this);
				if (!mind) {
					makeBot();
					mind = UMindComponent::get(this);
				}
				if (mind) {
					mind->readXML(childNode);
				}
			}
		}
	}
}


ACreature* ACreature::spawn(UWorld* world, FString specKey, FVector position, float yaw) {
	UCreatureSpec* spec = UResourceCache::getCreatureSpec(specKey);
	if (spec == nullptr) {
		UDebug::error("Could not find Creature Spec '" + specKey + "'");
		return nullptr;
	}
	return spawn(world, spec, position, yaw);
}

ACreature* ACreature::spawn(UWorld* world, UCreatureSpec* spec, FVector position, float yaw) {

	if (spec == nullptr) {
		UDebug::error("Could not spawn Creature, because its Spec was missing.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	UClass* creatureClass = ACreature::StaticClass();
	if (spec->creatureClass != nullptr) {
		creatureClass = spec->creatureClass;
	}

	ACreature* creature = world->SpawnActor<ACreature>(creatureClass, position, FRotator(0.0f, 0.0f, 0.0f), params);
	if (creature) {
		creature->spec = spec;
		creature->SetActorLocation(position);
		creature->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		creature->bodyYaw = yaw;
		creature->controlRotation.Yaw = yaw + 90.0f;
		creature->ready = true;

		if (!instances.Contains(creature)) {
			instances.Add(creature);
		}

	}
	else {
		UDebug::error("Spawn creature, " + spec->key + " failed!");
	}
	return creature;
}

ACreature* ACreature::spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw) {
	if (spawnClass == nullptr) {
		UDebug::error("Could not spawn Creature, because its Class was missing.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	ACreature* creature = world->SpawnActor<ACreature>(spawnClass, position, FRotator(0.0f, 0.0f, 0.0f), params);
	if (creature) {
		creature->SetActorLocation(position);
		creature->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		creature->bodyYaw = yaw;
		creature->controlRotation.Yaw = yaw + 90.0f;
		creature->ready = true;

		if (!instances.Contains(creature)) {
			instances.Add(creature);
		}

	}
	else {
		UDebug::error("Spawn creature, " + spawnClass->GetName() + " failed!");
	}
	return creature;
}

void ACreature::destroyAll() {
	TArray<ACreature*> instancesCopy = instances;
	for (ACreature* creature : instancesCopy) {
		if (creature) {
			creature->Destroy();
		}
	}
	instances.Empty();
}