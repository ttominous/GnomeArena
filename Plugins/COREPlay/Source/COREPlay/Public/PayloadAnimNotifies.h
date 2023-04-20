#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "Payload.h"

#include "PayloadAnimNotifies.generated.h"

class ACreature;


UENUM(BlueprintType)
enum class EAnimNotifyHandle : uint8 {
	PROCESSED = 0                   UMETA(DisplayName = "Processed"),
	EXITED = 1						UMETA(DisplayName = "Exited"),
};

UCLASS()
class COREPLAY_API UPayloadAnimNotify : public UAnimNotify {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FPayload payload;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > bonePositionsToLog;

	virtual void Notify(USkeletalMeshComponent* bodyMesh, UAnimSequenceBase* animation, const FAnimNotifyEventReference& EventReference) override;
};


UCLASS()
class COREPLAY_API UPingAnimNotify : public UPayloadAnimNotify {
	GENERATED_BODY()
public:
	UPingAnimNotify();
};


UCLASS()
class COREPLAY_API UEndAnimNotify : public UPayloadAnimNotify {
	GENERATED_BODY()
public:
	UEndAnimNotify();
};


UCLASS()
class COREPLAY_API ULaunchAnimNotify : public UPayloadAnimNotify {
	GENERATED_BODY()
public:
	ULaunchAnimNotify();
};

UCLASS()
class COREPLAY_API UFreezeAnimNotify : public UPayloadAnimNotify {
	GENERATED_BODY()
public:
	UFreezeAnimNotify();
};


UCLASS()
class COREPLAY_API UJumpAnimNotify : public UPayloadAnimNotify {
	GENERATED_BODY()
public:
	UJumpAnimNotify();
};


UCLASS()
class COREPLAY_API UMoveState : public UAnimNotifyState {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
		FVector direction;

	UPROPERTY(EditAnywhere)
		bool inBodyDirection = true;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;

};