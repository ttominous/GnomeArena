#pragma once

#include "CoreMinimal.h"
#include "ComplexUserWidget.h"

#include "CommonWidgets.generated.h"


UCLASS()
class COREPLAY_API UComplexButton : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString displayText;

	UPROPERTY(EditAnywhere)
		FColor displayTextColor;

	UPROPERTY(EditAnywhere)
		int displayTextSize;

	UPROPERTY(EditAnywhere)
		FColor displayBackgroundColor;

	UPROPERTY(EditAnywhere)
		UTexture* displayIcon = nullptr;

	UPROPERTY(EditAnywhere)
		TMap< EWidgetStyleState, FLinearColor > displayTextStyleColors;

	UPROPERTY(EditAnywhere)
		TMap< EWidgetStyleState, FLinearColor > backgroundStyleColors;

	UPROPERTY(EditAnywhere)
		FVector2D minSize;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputDelegate onSelectWithChild;

	virtual void applyStyle() override;

};

UCLASS()
class COREPLAY_API ULabeledInput : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString labelValue;

	UPROPERTY()
		bool updating = false;

	UFUNCTION(BlueprintCallable)
		virtual void setLabel(FString label);

	virtual void init() override;

};

UCLASS()
class COREPLAY_API USliderBar : public ULabeledInput {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString minLabelValue;

	UPROPERTY(EditAnywhere)
		FString maxLabelValue;

	UPROPERTY(EditAnywhere)
		float min = 0.0f;

	UPROPERTY(EditAnywhere)
		float max = 1.0f;

	UPROPERTY(EditAnywhere)
		float currentValue = 0.0f;

	UPROPERTY(EditAnywhere)
		float stepSize = 0.05;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputNumericDelegate onValueChanged;


	UFUNCTION(BlueprintCallable)
		virtual void setMinLabel(FString inMinLabel);

	UFUNCTION(BlueprintCallable)
		virtual void setMaxLabel(FString inMaxLabel);

	UFUNCTION(BlueprintCallable)
		virtual void setMin(float inMin);

	UFUNCTION(BlueprintCallable)
		virtual void setMax(float inMax);

	UFUNCTION(BlueprintCallable)
		virtual void setValue(float inValue);

	UFUNCTION(BlueprintCallable)
		virtual void setStep(float inStepSize);

	virtual FString getDisplayValue();

	virtual void init() override;

	virtual void handleInputPress(const FKey Key) override;


};

UCLASS()
class COREPLAY_API UArrowSelect : public ULabeledInput {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString value;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > values;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputStringDelegate onValueChanged;


	UFUNCTION(BlueprintCallable)
		virtual void setValue(FString inValue);

	UFUNCTION(BlueprintCallable)
		virtual void setValueFromFinalValue(FString inValue);

	UFUNCTION(BlueprintCallable)
		virtual void setValues(TMap< FString, FString > inValues);

	UFUNCTION(BlueprintCallable)
		virtual void setValuesFromArray(TArray< FString > inValues);

	UFUNCTION(BlueprintCallable)
		virtual FString getFinalValue();

	UFUNCTION(BlueprintCallable)
		virtual void nextValue();

	UFUNCTION(BlueprintCallable)
		virtual void previousValue();

	virtual void init() override;

	virtual void handleInputPress(const FKey Key) override;


};


UCLASS()
class COREPLAY_API UBooleanInput : public ULabeledInput {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		bool value = false;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputBoolDelegate onValueChanged;

	UFUNCTION(BlueprintCallable)
		virtual void setValue(bool inValue);

	UFUNCTION(BlueprintCallable)
		virtual bool getValue();

	UFUNCTION(BlueprintCallable)
		virtual void toggleValue();

};



UCLASS()
class COREPLAY_API UTextBox : public ULabeledInput {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString value;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputStringDelegate onValueChanged;


	UFUNCTION(BlueprintCallable)
		virtual void setValue(FString inValue);

	virtual void init() override;
	virtual void handleSelect() override;

	UFUNCTION()
		virtual void finishEditingFromVirtualKeyboard(UUserWidget* virtualKeyboard, FString newValue);

};



UCLASS()
class COREPLAY_API UColorPicker : public ULabeledInput {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FColor currentColor;

	UPROPERTY(EditAnywhere)
		FColor popupTitleColor = FColor::White;

	UPROPERTY(EditAnywhere)
		FString popupTitle = "Choose A Color";

	UPROPERTY(EditAnywhere)
		FColor popupMessageColor = FColor::White;

	UPROPERTY(EditAnywhere)
		FString popupMessage = "This color will be applied somewhere.";

	UPROPERTY(EditAnywhere)
		FString popupLayerKey = "ColorPicker";

	UPROPERTY(EditAnywhere)
		FString popupWidgetKey = "ColorPicker";

	UPROPERTY(EditAnywhere)
		TSubclassOf< UColorPickerPopup > popupWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
		int popupZIndex = 100;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputColorDelegate onColorChanged;

	UFUNCTION(BlueprintCallable)
		virtual void setColor(FColor color);

	virtual void refreshPreview();

	UFUNCTION(BlueprintCallable)
		virtual void startEditing();

	UFUNCTION(BlueprintCallable)
		virtual void finishEditing(UUserWidget* instigator, FColor color);

	virtual void init() override;

};


UCLASS()
class COREPLAY_API UPopUp : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FColor titleColorValue = FColor::White;

	UPROPERTY(EditAnywhere)
		FString titleValue;

	UPROPERTY(EditAnywhere)
		FColor messageColorValue = FColor::White;

	UPROPERTY(EditAnywhere)
		FString messageValue;


	virtual void init() override;

};

UCLASS()
class COREPLAY_API UColorPickerPopup : public UPopUp {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FColor currentColor;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputDelegate onCancel;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputColorDelegate onFinish;

	UPROPERTY()
		bool updating = false;

	UFUNCTION(BlueprintCallable)
		virtual void updateRed(float value);

	UFUNCTION(BlueprintCallable)
		virtual void updateGreen(float value);

	UFUNCTION(BlueprintCallable)
		virtual void updateBlue(float value);

	virtual void refreshPreview();

	UFUNCTION(BlueprintCallable)
		virtual void finish();

	UFUNCTION(BlueprintCallable)
		virtual void cancel();

	virtual void init() override;

	UFUNCTION(BlueprintCallable)
		static UColorPickerPopup* open(FString layerKey, FString widgetKey, int zLayer, APlayerController* playerController, UClass* popupClass, FColor initialColor, FString title, FColor titleColor, FString message, FColor messageColor);

};
