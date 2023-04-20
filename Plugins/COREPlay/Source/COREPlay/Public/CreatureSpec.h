#pragma once

#include "CoreMinimal.h"
#include "FXList.h"
#include "DecalSpec.h"
#include "ItemSet.h"
#include "RagdollAnimInstance.h"
#include "SlotAnimState.h"

#include "CreatureSpec.generated.h"

class ACreature;
class UMindComponent;

UCLASS(Blueprintable)
class COREPLAY_API UCreatureSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ACreature> creatureClass = nullptr;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UMindComponent> mindComponentClass = nullptr;

	UPROPERTY(EditAnywhere)
		TMap< FString, UFXList* > hitEffects;

	UPROPERTY(EditAnywhere)
		TMap< FString, UDecalSpec* > hitBloodDecals;

	UPROPERTY(EditAnywhere)
		TArray< FString > ragdollReplicatedBones;

	UPROPERTY(EditAnywhere)
		TSubclassOf< URagdollAnimInstance > ragdollAnimInstance = nullptr;

	UPROPERTY(EditAnywhere)
		float despawnTime = 10.0f;

	UPROPERTY(EditAnywhere)
		FItemSet spawnItems;

	UPROPERTY(EditAnywhere)
		bool spawnMindIfUncontrolled = false;

	UPROPERTY(EditAnywhere)
		float staggerResistance = 0.0f;

	UPROPERTY(EditAnywhere)
		float forceResistance = 0.0f;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > defaultAimBoneAmounts;

	UPROPERTY(EditAnywhere)
		bool defaultAimRotationEnabled = false;

	UPROPERTY(EditAnywhere)
		ESlotAnimState defaultTorsoSlotState;

	UPROPERTY(EditAnywhere)
		bool defaultWeaponHandSnapEnabled = false;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > defaultIdleArmsBlendAmounts;
};
