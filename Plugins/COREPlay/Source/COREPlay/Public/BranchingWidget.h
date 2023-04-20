#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ComplexUserWidget.h"
#include "UITools.h"
#include "Payload.h"

#include "BranchingWidget.generated.h"


UCLASS()
class COREPLAY_API UBranchingWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		TArray< UClass* > previousWidgetClasses;

	UPROPERTY()
		TArray< FPayload > previousWidgetData;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > startingWidgetClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
		UUserWidget* currentWidget = nullptr;

	UPROPERTY()
		UBorder* widgetContainer = nullptr;

	UPROPERTY()
		TSubclassOf< UUserWidget > currentWidgetClass = nullptr;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual UBorder* getWidgetContainer();

	UFUNCTION(BlueprintCallable)
		virtual void proceedToNext(UClass* nextClass, FPayload previousData);

	UFUNCTION(BlueprintCallable)
		virtual void backToPrevious();

	UFUNCTION(BlueprintCallable)
		static void proceedToNextFromChild(UUserWidget* child, UClass* nextClass, FPayload previousData);
	
	UFUNCTION(BlueprintCallable)
		static void backToPreviousFromChild(UUserWidget* child);

};