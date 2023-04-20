#include "PhysicsMorphComponent.h"
#include "BodyComponent.h"
#include "ResourceCache.h"
#include "MathTools.h"
#include "Debug.h"

#include "GameFramework/Character.h"

UPhysicsMorphComponent::UPhysicsMorphComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhysicsMorphComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector gravity = FVector(0, 0.0f, -1.0f);

	for (auto& entry : slots) {
		FPhysicsMorphSlot& slot = entry.Value;
		if (slot.meshes.Num() == 0) {
			continue;
		}

		USkeletalMeshComponent* mesh = slot.meshes[0];
		if (mesh == nullptr) {
			continue;
		}
		FVector position = mesh->GetSocketLocation(slot.socketName);
		FRotator rotation = mesh->GetSocketRotation(slot.socketName);

		FVector upDir = rotation.RotateVector(FVector(0.0f, -1.0f, 0.0f));
		float up = 0.0f * slot.gravityPerc;

		FVector forwardDir = rotation.RotateVector(FVector(0.0f, 0.0f, 1.0f));
		float forward = forwardDir.Dot(gravity) * slot.gravityPerc;

		FVector leftDir = rotation.RotateVector(FVector(1.0f, 0.0f, 0.0f));
		float left = leftDir.Dot(gravity) * slot.gravityPerc;


		FVector moveDir = position - slot.lastPos;
		float moveAmount = moveDir.Size() / slot.distance;
		moveDir.Normalize();
		slot.lastPos = position;

		forward -= forwardDir.Dot(moveDir) * moveAmount * slot.motionPerc;
		left -= leftDir.Dot(moveDir) * moveAmount * slot.motionPerc;
		up -= upDir.Dot(moveDir) * moveAmount * slot.motionPerc;


		float normalPerc = FMath::Abs(forward) + FMath::Abs(left) + FMath::Abs(up);
		if (normalPerc > 1.0f) {
			normalPerc = 1.0f / normalPerc;
			forward *= normalPerc;
			left *= normalPerc;
			up *= normalPerc;
		}

		for (int i = 0; i < slot.impulses.Num(); i++) {
			float impulseSize = slot.impulses[i].impulse.Size() / slot.distance;

			FVector impulseDir = slot.impulses[i].impulse;
			if (slot.impulses[i].localSpace) {
				impulseDir = rotation.RotateVector(impulseDir);
			}

			forward -= forwardDir.Dot(impulseDir) * impulseSize * slot.motionPerc;
			left -= leftDir.Dot(impulseDir) * impulseSize * slot.motionPerc;
			up -= upDir.Dot(impulseDir) * impulseSize * slot.motionPerc;

			slot.impulses[i].impulse = UMathTools::lerpVectorByPerc(slot.impulses[i].impulse, FVector::ZeroVector, slot.impulses[i].degradeRate * DeltaTime);
			if (slot.impulses[i].impulse.Size() <= 0.01f) {
				slot.impulses.RemoveAt(i);
				i--;
			}
		}


		slot.forward = UMathTools::lerpFloatByPerc(slot.forward, forward, DeltaTime * slot.lerpRate);
		slot.left = UMathTools::lerpFloatByPerc(slot.left, left, DeltaTime * slot.lerpRate);
		slot.up = UMathTools::lerpFloatByPerc(slot.up, up, DeltaTime * slot.lerpRate);

		mesh->SetMorphTarget(slot.forwardMorphName, FMath::Clamp(slot.forward, 0.0f, 1.0f), true);
		mesh->SetMorphTarget(slot.backMorphName, FMath::Clamp(-slot.forward, 0.0f, 1.0f), true);
		mesh->SetMorphTarget(slot.leftMorphName, FMath::Clamp(slot.left, 0.0f, 1.0f), true);
		mesh->SetMorphTarget(slot.rightMorphName, FMath::Clamp(-slot.left, 0.0f, 1.0f), true);
		mesh->SetMorphTarget(slot.upMorphName, FMath::Clamp(slot.up, 0.0f, 1.0f), true);
		mesh->SetMorphTarget(slot.downMorphName, FMath::Clamp(-slot.up, 0.0f, 1.0f), true);


		if (slot.drawDebug) {
			UDebug::drawLine(GetWorld(), position, position + forwardDir*50.0f, FColor::Red, 1.0f, 0.05f);
			UDebug::drawLine(GetWorld(), position, position + moveDir * 50.0f, FColor::Green, 1.0f, 0.05f);
		}

	}

}

void UPhysicsMorphComponent::rebuild(UBodyComponent* body) {

	for (auto entry : slots) {
		slots[entry.Key].meshes.Empty();
	}

	for (FBodyPartSlot& slot : body->slots) {

		USkeletalMeshComponent* component = nullptr;
		if (body->components.Contains(slot.key)) {
			component = Cast<USkeletalMeshComponent>(body->components[slot.key]);
		}
		if (component == nullptr) {
			continue;
		}

		UBodyPart* bodyPart = UResourceCache::getBodyPart(slot.bodyPartKey);
		if (bodyPart == nullptr) {
			continue;
		}
		FBodyPartDetail& details = bodyPart->getDetails(slot.key,body);

		for (FString morphSlot : details.physicsMorphSlots) {
			if (slots.Contains(morphSlot)) {
				slots[morphSlot].meshes.Add(component);
			}
		}
	}
}

void UPhysicsMorphComponent::addImpulse(FString slotKey, FVector amount, float degradeRate, bool localSpace) {
	if (slots.Contains(slotKey)) {
		FPhysicsMorphImpulse impulse;
		impulse.impulse = amount;
		impulse.degradeRate = degradeRate;
		impulse.localSpace = localSpace;
		slots[slotKey].impulses.Add(impulse);
	}
}


UPhysicsMorphComponent* UPhysicsMorphComponent::get(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	return Cast< UPhysicsMorphComponent >(actor->GetComponentByClass(UPhysicsMorphComponent::StaticClass()));
}
