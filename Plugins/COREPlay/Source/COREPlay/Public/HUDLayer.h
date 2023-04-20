#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "HUDLayer.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FHUDLayer {

	GENERATED_BODY()

public:

	UPROPERTY()
		TMap< FString, UUserWidget* > widgets;

	UPROPERTY()
		int zIndex = 0;

	UPROPERTY()
		TArray< FString > focusableWidgets;
};