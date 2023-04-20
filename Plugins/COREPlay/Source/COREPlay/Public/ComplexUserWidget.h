#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ComplexUserWidget.generated.h"

class AComplexHUD;

UENUM(BlueprintType)
enum class EWidgetStyleState : uint8 {
	DEFAULT = 0				          UMETA(DisplayName = "Default"),
	HOVERED = 1                       UMETA(DisplayName = "Hovered"),
	DISABLED = 2					  UMETA(DisplayName = "Disabled"),
	DISABLED_HOVERED = 3              UMETA(DisplayName = "DisabledHovered"),
	SELECTED = 4					  UMETA(DisplayName = "Selected"),
	SELECTED_HOVERED = 5              UMETA(DisplayName = "SelectedHovered")
};

UCLASS()
class COREPLAY_API UComplexUserWidget : public UUserWidget {

	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDelegate, class UUserWidget*, Child);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputNumericDelegate, class UUserWidget*, Child, double, Value);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputStringDelegate, class UUserWidget*, Child, FString, Value);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputColorDelegate, class UUserWidget*, Child, FColor, Value);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputBoolDelegate, class UUserWidget*, Child, bool, Value);


	UPROPERTY()
		EWidgetStyleState currentStyle;

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		bool styleDirty = true;

	UPROPERTY()
		bool hovered = false;

	UPROPERTY()
		bool selected = false;

	UPROPERTY(EditAnywhere)
		bool disabled = false;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputDelegate onSelectDelegate;

	UPROPERTY()
		FString path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* defaultGamePadFocus = nullptr;


	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void onInit();
	virtual void init();

	virtual EWidgetStyleState getStyle();
	UFUNCTION(BlueprintImplementableEvent)
		void onApplyStyle(EWidgetStyleState newStyle);
	virtual void applyStyle();


	UFUNCTION(BlueprintImplementableEvent)
		void onHandleGamePadUseChange(bool usingGamepad);
	virtual void handleGamePadUseChange(bool usingGamepad);

	UFUNCTION(BlueprintImplementableEvent)
		void onInputPress(const FString& key);
	virtual void handleInputPress(const FKey Key);

	UFUNCTION(BlueprintImplementableEvent)
		void onInputRelease(const FString& key);
	virtual void handleInputRelease(const FKey Key);

	UFUNCTION(BlueprintImplementableEvent)
		void onHandleSelect();
	virtual void handleSelect();
	UFUNCTION(BlueprintCallable)
		virtual void triggerSelect();

	UFUNCTION(BlueprintCallable)
		virtual bool canInteract();
	virtual bool canInteract(AComplexHUD* hud);

	UFUNCTION(BlueprintCallable)
		virtual void close();

	UFUNCTION(BlueprintCallable)
		virtual void setSelected(bool isSelected);

	UFUNCTION(BlueprintCallable)
		virtual void setDisabled(bool isDisabled);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		virtual bool isDisabled();

};