#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetList.generated.h"



USTRUCT(BlueprintType)
struct COREPLAY_API FWidgetListEntry {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget> widgetClass;

	UPROPERTY(EditAnywhere)
		bool focusable = false;
};

USTRUCT(BlueprintType)
struct COREPLAY_API FWidgetList {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FWidgetListEntry > widgets;
};