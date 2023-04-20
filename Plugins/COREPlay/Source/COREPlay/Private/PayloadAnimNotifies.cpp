#include "PayloadAnimNotifies.h"
#include "Creature.h"
#include "Debug.h"

void UPayloadAnimNotify::Notify(USkeletalMeshComponent* bodyMesh, UAnimSequenceBase* animation, const FAnimNotifyEventReference& EventReference ) {
	Super::Notify(bodyMesh, animation, EventReference);

	if (bodyMesh && bodyMesh->GetOwner() && bodyMesh->GetOwner()->IsA(ACreature::StaticClass())) {

		FPayload finalPayload = payload;;
		finalPayload.setString("animSequence", animation->GetName());

		ACreature* creature = Cast< ACreature >(bodyMesh->GetOwner());

		if (bonePositionsToLog.Num() > 0) {
			for (auto& boneKey : bonePositionsToLog) {
				FVector boneLocation = creature->GetMesh()->GetSocketLocation(FName(*boneKey.Key));
				finalPayload.setVector(boneKey.Value, boneLocation);
			}
		}

		creature->handlePayload(finalPayload);
	}
}

UPingAnimNotify::UPingAnimNotify() {
	payload.setString("type", FPayload::TYPE_ANIM_PING);
}

UEndAnimNotify::UEndAnimNotify() {
	payload.setString("type", FPayload::TYPE_ANIM_END);
}

ULaunchAnimNotify::ULaunchAnimNotify() {
	payload.setString("type", FPayload::TYPE_LAUNCH);
	payload.setBool("inBodyDirection", true);
	payload.setVector("direction", FVector::ZeroVector);
}

UFreezeAnimNotify::UFreezeAnimNotify() {
	payload.setString("type", FPayload::TYPE_FREEZE_ANIM);
}

UJumpAnimNotify::UJumpAnimNotify() {
	payload.setString("type", FPayload::TYPE_JUMP);
}


void UMoveState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) {
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	if (MeshComp && MeshComp->GetOwner() && MeshComp->GetOwner()->IsA(ACreature::StaticClass())) {
		ACreature* creature = Cast< ACreature >(MeshComp->GetOwner());
		if (inBodyDirection) {
			creature->LaunchCharacter(FRotator(0.0f, creature->bodyYaw, 0.0f).RotateVector(direction), false, false);
		}
		else {
			creature->LaunchCharacter(direction, false, false);
		}
	}
}