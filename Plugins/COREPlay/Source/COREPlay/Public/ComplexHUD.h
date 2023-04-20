#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"

#include "HUDLayer.h"

#include "ComplexHUD.generated.h"


UCLASS()
class COREPLAY_API AComplexHUD : public AHUD {

	GENERATED_BODY()

public:

	DECLARE_DELEGATE_OneParam(FAnyKeyDelegate, FString);

	static FString STATE_DEFAULT;
	static FString STATE_PAUSED;

	static FString LAYER_DEFAULT;

	UPROPERTY()
		TMap< FString, FHUDLayer > widgetLayers;

	UPROPERTY()
		FString currentState = STATE_DEFAULT;

	UPROPERTY()
		UUserWidget* currentFocusedWidget = nullptr;

	UPROPERTY()
		UUserWidget* mouseHoveredWidget = nullptr;

	UPROPERTY()
		UUserWidget* virtualHoverWidget = nullptr;

	UPROPERTY()
		UUserWidget* lastPrompt = nullptr;

	UPROPERTY()
		UUserWidget* lastVirtualInput = nullptr;

	UPROPERTY()
		bool isGamePad = false;

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		TMap< FString, bool > interactableCache;

	AComplexHUD();
	virtual void Tick(float DeltaTime) override;

	void removeAllLayers(bool allowFocusChangeUpdate = true);

	bool hasLayer(FString layerKey);
	void addLayer(FString layerKey, int zIndex = 0);
	void removeLayer(FString layerKey, bool allowFocusChangeUpdate = true);
	void checkLayer(FString layerKey, int zIndex = 0, bool shouldHave = true);

	bool hasWidget(FString layerKey, FString widgetKey);
	UUserWidget* addWidget(FString layerKey, FString widgetKey, FString widgetClassKey, int zIndex = 0, bool hasFocus = false, bool allowFocusChangeUpdate = true);
	UUserWidget* addWidget(FString layerKey, FString widgetKey, UClass* widgetClass, int zIndex = 0, bool hasFocus = false, bool allowFocusChangeUpdate = true);
	void removeWidget(FString layerKey, FString widgetKey, bool allowFocusChangeUpdate = true);
	void removeWidget(UUserWidget* widget, bool allowFocusChangeUpdate = true);
	void checkWidget(FString layerKey, FString widgetKey, FString widgetClassKey, int zIndex = 0, bool hasFocus = false, bool shouldHave = true);
	UUserWidget* getWidget(FString layerKey, FString widgetKey);
	UUserWidget* getWidget(UClass* targetClass);

	virtual void applyWidgetList(FString layerKey, FString widgetListKey, int zIndex = 0, bool allowFocusChangeUpdate = true);
	virtual FString getDefaultWidgetListKey();

	UFUNCTION(BlueprintCallable)
		virtual UUserWidget* prompt(FString widgetKey);

	UFUNCTION(BlueprintCallable)
		virtual void removePrompt(FString widgetKey);

	UFUNCTION(BlueprintCallable)
		virtual UUserWidget* openVirtualInput(FString widgetKey);

	UFUNCTION(BlueprintCallable)
		virtual void closeVirtualInput(FString widgetKey);

	virtual void setState(FString newState);
	virtual void applyState();

	virtual void handlePossess(APawn* pawn);
	virtual void handleUnPossess(APawn* pawn);
	virtual void handleGamepadUseSwitch(bool usingGamepad);

	virtual void updateFocus();
	virtual void setVirtualFocus(UUserWidget* widget);
	virtual void rebindInputs(UInputComponent* inputComponent);

	UFUNCTION()
		virtual void pressAnyKey(FString input);

	UFUNCTION()
		virtual void releaseAnyKey(FString input);

	static AComplexHUD* get(APawn* pawn);
	static AComplexHUD* get(APlayerController* controller);
	static void Possess(APawn* pawn);
	static void UnPossess(APawn* pawn);

};