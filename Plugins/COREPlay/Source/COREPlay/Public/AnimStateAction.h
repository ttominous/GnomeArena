#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionInstance.h"
#include "PlayAnimationData.h"

#include "AnimStateAction.generated.h"

class UActionInstance;
class UActionComponent;

UCLASS()
class COREPLAY_API UAnimStateAction : public UAction {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FPlayAnimationData animationData;

	UAnimStateAction() {

	}

	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) override;
	virtual void processEnd(UActionInstance* instance, UActionComponent* component) override;

};