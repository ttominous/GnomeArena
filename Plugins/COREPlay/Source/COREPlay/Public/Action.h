#pragma once

#include "CoreMinimal.h"
#include "Payload.h"
#include "PlayAnimationData.h"
#include "TargetDetail.h"
#include "XMLTools.h"

#include "Action.generated.h"

class UActionInstance;
class UActionComponent;
class ACreature;

UCLASS(Blueprintable)
class COREPLAY_API UAction : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		UTexture2D* iconTexture = nullptr;

	UPROPERTY(EditAnywhere)
		FString iconKey;

	UPROPERTY(EditAnywhere)
		bool bindToLeader = true;

	UPROPERTY(EditAnywhere)
		bool allowMovement = false;

	UPROPERTY(EditAnywhere)
		bool allowJumping = false;

	UPROPERTY(EditAnywhere)
		bool allowSprinting = false;

	UPROPERTY(EditAnywhere)
		bool allowAiming = false;

	UPROPERTY(EditAnywhere)
		bool allowHandSwitching = false;

	UPROPERTY(EditAnywhere)
		bool allowTurning = false;

	UPROPERTY(EditAnywhere)
		bool allowWeaponHandSnapping = false;

	UPROPERTY(EditAnywhere)
		bool allowCrouching = false;

	UPROPERTY(EditAnywhere)
		bool allowCasualInterupts = false;

	UPROPERTY(EditAnywhere)
		bool canBeDoneFromVessel = false;

	UPROPERTY(EditAnywhere)
		bool ticks = false;

	UPROPERTY(EditAnywhere)
		float aimAccuracyMultiplier = 1.0f;

	UPROPERTY(EditAnywhere)
		bool persistInSave = false;

	virtual bool allowMove(UActionInstance* instance);
	virtual bool allowJump(UActionInstance* instance);
	virtual bool allowSprint(UActionInstance* instance);
	virtual bool allowAim(UActionInstance* instance);
	virtual bool allowSwitchHands(UActionInstance* instance);
	virtual bool allowTurn(UActionInstance* instance);
	virtual bool allowWeaponHandSnap(UActionInstance* instance);
	virtual bool allowCrouch(UActionInstance* instance);
	virtual float getAimAccuracy(UActionInstance* instance);

	virtual bool canDoAction(AActor* actor);
	virtual float getCompletionPerc(UActionInstance* instance, UActionComponent* component);

	virtual bool processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload);
	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target);
	virtual void processDeferredStart(UActionInstance* instance, UActionComponent* component);
	virtual void processExecute(UActionInstance* instance, UActionComponent* component, FTargetDetail target);
	virtual void processEnd(UActionInstance* instance, UActionComponent* component);
	virtual void processInterupt(UActionInstance* instance, UActionComponent* component, FVector targetPosition);
	virtual void processTick(float deltaTime, UActionInstance* instance, UActionComponent* component);
	virtual void processInputPress(FString inputKey, UActionInstance* instance, UActionComponent* component);
	virtual void processInputRelease(FString inputKey, UActionInstance* instance, UActionComponent* component);
	virtual bool isRelevantInput(FString inputKey);

	virtual FString playAnimation(FPlayAnimationData& animationData, ACreature* creature);

	virtual UClass* getInstanceClass(UActionComponent* component);
	virtual UActionInstance* spawnInstance(UActionComponent* component);

	virtual void writeXML(FXMLBuilderNode& rootNode, UActionInstance* instance, UActionComponent* component);
	virtual void readXML(FXmlNode* rootNode, UActionInstance* instance, UActionComponent* component);

};
