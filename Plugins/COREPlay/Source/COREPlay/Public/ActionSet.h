#pragma once

#include "CoreMinimal.h"
#include "Action.h"

#include "ActionSet.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UActionSet : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, UAction* > actions;

	UAction* getAction(FString actionKey);

};
