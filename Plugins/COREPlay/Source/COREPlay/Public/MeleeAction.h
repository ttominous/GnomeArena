#pragma once

#include "CoreMinimal.h"
#include "PlayAnimAction.h"
#include "DamageDetails.h"

#include "MeleeAction.generated.h"

UCLASS()
class COREPLAY_API UMeleeAction : public UPlayAnimAction {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FDamageDetails baseDamage;

	UPROPERTY(EditAnywhere)
		FString nextComboActionKey = "";

	UPROPERTY(EditAnywhere)
		bool useControlPitch = false;

	UPROPERTY(EditAnywhere)
		FString comboInputKey = "PrimaryAttack";

	UPROPERTY(EditAnywhere)
		bool debug = false;

	virtual void processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload);

	virtual FString playAnimation(FPlayAnimationData& animationData, ACreature* creature) override;
	virtual void processInputPress(FString inputKey, UActionInstance* instance, UActionComponent* component) override;
	virtual bool isRelevantInput(FString inputKey) override;

	virtual UClass* getInstanceClass(UActionComponent* component) override;

};




UCLASS(Blueprintable)
class COREPLAY_API UMeleeActionInstance : public UPlayAnimActionInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool damageApplied = false;

	UPROPERTY()
		bool comboRequested = false;

};