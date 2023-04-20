#pragma once

#include "CoreMinimal.h"
#include "ComplexUserWidget.h"

#include "UITools.h"

#include "SettingsWidget.generated.h"


UCLASS()
class COREPLAY_API USettingsWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		TArray< FString > commonLowToHighValues;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void init() override;

	UFUNCTION(BlueprintCallable)
		virtual void onBooleanChange(UUserWidget* instigator, bool newValue);

	UFUNCTION(BlueprintCallable)
		virtual void onStringChange(UUserWidget* instigator, FString newValue);

	UFUNCTION(BlueprintCallable)
		virtual void onNumericChange(UUserWidget* instigator, double newValue);



	UFUNCTION(BlueprintCallable)
		virtual void saveChanges();

	UFUNCTION(BlueprintCallable)
		virtual void cancelUnappliedChanges();

	virtual void refreshSettings();
	virtual void refreshApplyChanges();

};