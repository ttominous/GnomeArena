#include "RagdollComponent.h"
#include "Creature.h"
#include "CreatureSpec.h"
#include "BodyComponent.h"
#include "CollisionTools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"


FTransform FRagdollBoneTransform::getTransform() {
	FTransform transform;
	transform.SetLocation(FVector(location.X, location.Y, location.Z));
	transform.SetRotation(FRotator(rotation.X, rotation.Y, rotation.Z).Quaternion());
	transform.SetScale3D(FVector(1, 1, 1));
	return transform;
}

void FRagdollBoneTransform::set(FTransform& transform) {
	FVector transformLocation = transform.GetLocation();
	FRotator transformRotation = transform.GetRotation().Rotator();
	location.X = transformLocation.X;
	location.Y = transformLocation.Y;
	location.Z = transformLocation.Z;
	rotation.X = transformRotation.Pitch;
	rotation.Y = transformRotation.Yaw;
	rotation.Z = transformRotation.Roll;
}

FRagdollBoneTransform FRagdollBoneTransform::fromTransform(FTransform& transform) {
	FRagdollBoneTransform result;
	result.set(transform);
	return result;
}


URagdollComponent::URagdollComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void URagdollComponent::init() {
	if (GetOwner()->IsA(ACharacter::StaticClass())) {
		meshComponent = ((ACharacter*)GetOwner())->GetMesh();
		originalAnimInstanceClass = meshComponent->GetAnimInstance()->GetClass();
	}

	ACreature* creature = Cast< ACreature >(GetOwner());
	if (creature && creature->spec) {

		rootBone = creature->spec->ragdollReplicatedBones.Num() > 0 ? creature->spec->ragdollReplicatedBones[0] : rootBone;
		boneTransforms.Empty();
		for (FString boneName : creature->spec->ragdollReplicatedBones) {
			FTransform transform = meshComponent->GetSocketTransform(FName(*boneName));
			FRagdollBoneTransform boneTransform = FRagdollBoneTransform::fromTransform(transform);
			boneTransform.boneName = boneName;
			boneTransforms.Add(boneTransform);
		}

		UCapsuleComponent* capsuleComponent = creature->GetCapsuleComponent();
		capsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		((UCharacterMovementComponent*)creature->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_None);
		creature->GetMovementComponent()->Deactivate();

		meshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		meshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		meshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		meshComponent->GetBodyInstance()->bUseCCD = true;

		if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
			meshComponent->SetAllBodiesBelowSimulatePhysics(FName(*rootBone), true, true);
			meshComponent->SetAllBodiesBelowPhysicsBlendWeight(FName(*rootBone), 1.0f, true, true);
		}
		else {
			meshComponent->SetAnimInstanceClass(creature->spec->ragdollAnimInstance);
		}
	}

	initialized = true;
}

void URagdollComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	if (!initialized || !meshComponent) {
		Super::EndPlay(EndPlayReason);
		return;
	}

	if (GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* creature = Cast<ACreature>(GetOwner());
		UCapsuleComponent* capsuleComponent = creature->GetCapsuleComponent();
		capsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		meshComponent->AttachToComponent(capsuleComponent, FAttachmentTransformRules::KeepRelativeTransform);
		meshComponent->SetRelativeLocation(FVector(0, 0, -creature->getCapsuleHalfHeight() - 3.5f));
		meshComponent->GetBodyInstance()->bUseCCD = false;
		creature->CacheInitialMeshOffset(meshComponent->GetRelativeLocation(), meshComponent->GetRelativeRotation());

		((UCharacterMovementComponent*)creature->GetMovementComponent())->SetMovementMode(EMovementMode::MOVE_Walking);
		creature->GetMovementComponent()->Activate();
	}

	if (originalAnimInstanceClass != nullptr) {
		meshComponent->SetAnimInstanceClass(originalAnimInstanceClass);
	}

	UBodyComponent* body = UBodyComponent::get(GetOwner());
	if (body) {
		body->applyRootDefaultPhysics();
	}
	else {
		meshComponent->SetAllBodiesBelowSimulatePhysics(FName(*rootBone), false, true);
		meshComponent->SetAllBodiesBelowPhysicsBlendWeight(FName(*rootBone), 0.0f, false, true);
	}

	Super::EndPlay(EndPlayReason);
}

void URagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!initialized) {
		init();
	}

	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		updateTimer -= DeltaTime;
		if (highFrequencyUpdateTimer > 0.0f) {
			highFrequencyUpdateTimer -= DeltaTime;
		}
		bool bigChange = false;
		if (updateTimer <= 0.0f) {
			for (FRagdollBoneTransform& boneTransform : boneTransforms) {

				FTransform newTransform = meshComponent->GetSocketTransform(FName(*boneTransform.boneName));
				FTransform previousTransform = boneTransform.getTransform();

				if (!bigChange && FVector::Dist(newTransform.GetLocation(), previousTransform.GetLocation()) > 10.0f) {
					bigChange = true;
					highFrequencyUpdateTimer = FMath::Max(highFrequencyUpdateTimer,0.5f);
				}
				boneTransform.set(newTransform);
			}
			sendBoneTransforms(boneTransforms);
			updateTimer = highFrequencyUpdateTimer > 0.0f ? 0.05f : 1.0f;
		}
	}

	if (GetOwner()->IsA(ACreature::StaticClass()) && ((ACreature*)GetOwner())->dead ) {
		UCapsuleComponent* capsuleComponent = ((ACreature*)GetOwner())->GetCapsuleComponent();
		capsuleComponent->SetWorldLocation(meshComponent->GetSocketLocation(FName(*rootBone)));
	}
}

void URagdollComponent::sendBoneTransforms_Implementation(const TArray< FRagdollBoneTransform >& inBoneTransforms) {
	applyBoneTransforms(inBoneTransforms);
}

void URagdollComponent::applyBoneTransforms(const TArray< FRagdollBoneTransform >& inBoneTransforms) {
	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		return;
	}
	boneTransforms = inBoneTransforms;
}


URagdollComponent* URagdollComponent::get(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	return Cast< URagdollComponent >(actor->GetComponentByClass(URagdollComponent::StaticClass()));
}

URagdollComponent* URagdollComponent::startRagdolling(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	URagdollComponent* ragdoll = get(actor);
	if (ragdoll) {
		return ragdoll;
	}

	ragdoll = NewObject< URagdollComponent >(actor);
	ragdoll->RegisterComponent();
	ragdoll->init();
	return ragdoll;
}

void URagdollComponent::stopRagdolling(AActor* actor) {
	if (actor == nullptr) {
		return;
	}
	URagdollComponent* ragdoll = get(actor);
	if (!ragdoll) {
		return;
	}
	ragdoll->DestroyComponent();
}

void URagdollComponent::expediteReplicate(AActor* actor) {
	if (actor == nullptr) {
		return;
	}
	URagdollComponent* ragdoll = get(actor);
	if (!ragdoll) {
		return;
	}
	ragdoll->updateTimer = 0.0f;
	ragdoll->highFrequencyUpdateTimer = 1.0f;
}