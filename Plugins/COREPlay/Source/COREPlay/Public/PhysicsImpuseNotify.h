#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "PhysicsImpuseNotify.generated.h"

class ACreature;


UCLASS()
class COREPLAY_API UPhysicsImpulseNotify : public UAnimNotify {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FString slotKey;

	UPROPERTY(EditAnywhere)
		FVector amount;

	UPROPERTY(EditAnywhere)
		float degradeRate = 1.0f;

	UPROPERTY(EditAnywhere)
		bool useBodyYaw = false;

	UPROPERTY(EditAnywhere)
		bool localSpace = true;

	virtual void Notify(class USkeletalMeshComponent* bodyMesh, class UAnimSequenceBase* animation, const FAnimNotifyEventReference& EventReference) override;
};
