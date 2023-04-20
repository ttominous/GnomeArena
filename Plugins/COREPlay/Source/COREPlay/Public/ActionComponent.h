#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Action.h"
#include "ActionInstance.h"
#include "ActionSet.h"
#include "PayLoad.h"
#include "TargetDetail.h"
#include "XMLTools.h"

#include "ActionComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UActionComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY()
		UActionInstance* baseActionInstance = nullptr;

	UPROPERTY(EditAnywhere)
		UActionSet* actionSet = nullptr;

	UPROPERTY(EditAnywhere)
		UActionSet* weaponActionSet = nullptr;

	TQueue< FPayload > payloads;

	UActionComponent();


	virtual bool allowMove();
	virtual bool allowJump();
	virtual bool allowSprint();
	virtual bool allowAim();
	virtual bool allowSwitchHands();
	virtual bool allowTurn();
	virtual bool allowCrouch();
	virtual float getAimAccuracy();

	virtual UAction* getAction(FString actionKey);

	virtual void queuePayload(FPayload payload);
	virtual bool processPayload(FPayload payload);

	virtual void startAction(FString actionKey);
	virtual void startAction(UAction* action);
	virtual void startAction(FString actionKey, FTargetDetail target);
	virtual void startAction(UAction* action, FTargetDetail target);
	virtual void startBaseAction(FString actionKey);
	virtual void startBaseAction(UAction* action);
	virtual void startBaseAction(FString actionKey, FTargetDetail target);
	virtual void startBaseAction(UAction* action, FTargetDetail target);
	virtual bool hasBaseAction(FString actionKey);

	virtual void stopAction(FString actionKey, UActionInstance* instance);
	virtual void stopBaseAction(FString actionKey);
	virtual void stopAllActions();

	virtual void interupt(FVector targetPosition);
	virtual void interuptBaseAction(FVector targetPosition);

	virtual void bindSharedAction(UAction* action, UActionInstance* instance);
	virtual void bindSharedBaseAction(UAction* action, UActionInstance* instance);

	virtual bool isBusy();
	virtual bool isBaseActionBusy();
	virtual bool canCasuallyInterupt();
	virtual bool canCasuallyInteruptBaseAction();

	virtual float getCompletionPerc();
	virtual float getCompletionPerc(UAction* action);
	virtual float getCompletionPerc(UActionInstance* instance, UAction* action);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void tickActions(float deltaTime);

	virtual void startProcessInputPress(FString inputKey);
	virtual void processInputPress(FString inputKey);
	virtual void sendKeyPress(FString inputKey);

	virtual void startProcessInputRelease(FString inputKey);
	virtual void processInputRelease(FString inputKey);
	virtual void sendKeyRelease(FString inputKey);

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

	static UActionComponent* get(AActor* actor);

};