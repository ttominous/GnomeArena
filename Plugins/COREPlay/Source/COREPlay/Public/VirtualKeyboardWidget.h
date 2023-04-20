#pragma once

#include "CoreMinimal.h"
#include "ComplexUserWidget.h"
#include "CommonWidgets.h"

#include "VirtualKeyboardWidget.generated.h"


UCLASS()
class COREPLAY_API UVirtualKeyboardWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString currentTextValue;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UComplexButton > buttonClass = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputDelegate onCancel;

	UPROPERTY(BlueprintAssignable, Category = "Appearance|Events")
		FOnInputStringDelegate onFinish;

	UPROPERTY()
		TArray< UComplexButton* > buttons;

	virtual void init() override;
	virtual void handleGamePadUseChange(bool usingGamepad) override;

	UFUNCTION(BlueprintCallable)
		virtual void cancel();

	UFUNCTION(BlueprintCallable)
		virtual void finish();

	virtual void setValue(FString value);
	virtual void setTitle(FString value);

	UFUNCTION()
		virtual void selectCharacter(UUserWidget* keyWidget);

};