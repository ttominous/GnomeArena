#pragma once

#include "CoreMinimal.h"

#include "ActionInstance.generated.h"

class UAction;

UCLASS(Blueprintable)
class COREPLAY_API UActionInstance : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY()
		UAction* action = nullptr;

	UPROPERTY()
		bool deferredStartComplete = false;

};
