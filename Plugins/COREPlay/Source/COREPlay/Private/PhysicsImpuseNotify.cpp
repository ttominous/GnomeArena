#include "PhysicsImpuseNotify.h"
#include "PhysicsMorphComponent.h"
#include "Creature.h"
#include "Debug.h"

void UPhysicsImpulseNotify::Notify(class USkeletalMeshComponent* bodyMesh, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& EventReference) {
	Super::Notify(bodyMesh, animation, EventReference);

	if (!bodyMesh || !bodyMesh->GetOwner()) {
		return;
	}

	UPhysicsMorphComponent* physicsMorphs = UPhysicsMorphComponent::get(bodyMesh->GetOwner());
	if (physicsMorphs == nullptr) {
		return;
	}

	FVector finalAmount = amount;
	if (useBodyYaw) {
		if (bodyMesh->GetOwner()->IsA(ACreature::StaticClass())) {
			ACreature* creature = Cast< ACreature >(bodyMesh->GetOwner());
			finalAmount = FRotator(0.0f, creature->bodyYaw, 0.0f).RotateVector(finalAmount);
		}
	}
	physicsMorphs->addImpulse(slotKey, finalAmount, degradeRate, localSpace);
}