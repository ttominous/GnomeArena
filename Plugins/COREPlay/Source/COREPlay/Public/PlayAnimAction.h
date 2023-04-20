#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionInstance.h"
#include "PlayAnimationData.h"

#include "PlayAnimAction.generated.h"

class UActionInstance;
class UActionComponent;

UCLASS()
class COREPLAY_API UPlayAnimAction : public UAction {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FPlayAnimationData playedAnimation;

	UPlayAnimAction() {
	}

	virtual bool processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) override;
	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) override;

	virtual void processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) override;


	virtual void processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload);
	virtual void processAnimationFinished(UActionInstance* instance, UActionComponent* component);

	virtual void processEnd(UActionInstance* instance, UActionComponent* component) override;

	virtual UClass* getInstanceClass(UActionComponent* component) override;

};




UCLASS(Blueprintable)
class COREPLAY_API UPlayAnimActionInstance : public UActionInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString animSequence;

	UPROPERTY()
		TArray< FString > pingLog;

};