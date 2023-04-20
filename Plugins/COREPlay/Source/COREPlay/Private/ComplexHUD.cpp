#include "ComplexHUD.h"
#include "ResourceCache.h"
#include "ControllerTools.h"
#include "ComplexPlayerController.h"
#include "ComplexGameState.h"
#include "ComplexUserWidget.h"
#include "CameraManagerComponent.h"
#include "DebugTextWidget.h"
#include "Debug.h"

#include "Widgets/SViewport.h"
#include "Blueprint/GameViewportSubsystem.h"


FString AComplexHUD::STATE_DEFAULT = "Default";
FString AComplexHUD::STATE_PAUSED = "Paused";
FString AComplexHUD::LAYER_DEFAULT = "Default";

AComplexHUD::AComplexHUD() {
	PrimaryActorTick.bCanEverTick = true;
}


void AComplexHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!initialized) {
		applyState();
	}
	else {
		bool invalid = false;
		for (auto widgetLayer : widgetLayers) {
			for (auto widget : widgetLayer.Value.widgets) {
				if (widget.Value == nullptr) {
					invalid = true;
					break;
				}
			}
		}
		if (invalid) {
			initialized = false;
		}
	}

}

void AComplexHUD::removeAllLayers(bool allowFocusChangeUpdate) {
	TArray< FString > widgetLayerKeys;
	widgetLayers.GetKeys(widgetLayerKeys);
	for (FString widgetLayerKey : widgetLayerKeys) {
		removeLayer(widgetLayerKey, false);
	}
	if (allowFocusChangeUpdate) {
		updateFocus();
	}
}

bool AComplexHUD::hasLayer(FString layerKey) {
	return widgetLayers.Contains(layerKey);
}

void AComplexHUD::addLayer(FString layerKey, int zIndex) {
	if (!widgetLayers.Contains(layerKey)) {
		FHUDLayer layer;
		layer.zIndex = zIndex;
		widgetLayers.Add(layerKey, layer);
	}
}

void AComplexHUD::removeLayer(FString layerKey, bool allowFocusChangeUpdate) {
	if (widgetLayers.Contains(layerKey)) {

		bool focusChanged = false;
		for (auto widget : widgetLayers[layerKey].widgets) {
			if (currentFocusedWidget == widget.Value) {
				currentFocusedWidget = nullptr;
				mouseHoveredWidget = nullptr;
				virtualHoverWidget = nullptr;
				focusChanged = true;
			}

			if (lastPrompt == widget.Value) {
				lastPrompt = nullptr;
			}
			if (lastVirtualInput == widget.Value) {
				lastVirtualInput = nullptr;
			}

			if (UDebugTextWidget::instance == widget.Value) {
				UDebugTextWidget::instance->dead = true;
				UDebugTextWidget::instance = nullptr;
			}
			widget.Value->RemoveFromParent();
		}

		widgetLayers.Remove(layerKey);
		if (focusChanged && allowFocusChangeUpdate) {
			updateFocus();
		}
	}
}

void AComplexHUD::checkLayer(FString layerKey, int zIndex, bool shouldHave) {
	if (shouldHave) {
		if (!hasLayer(layerKey)) {
			addLayer(layerKey, zIndex);
		}
	}
	else {
		if (hasLayer(layerKey)) {
			removeLayer(layerKey);
		}
	}
}



bool AComplexHUD::hasWidget(FString layerKey, FString widgetKey) {
	return widgetLayers.Contains(layerKey) && widgetLayers[layerKey].widgets.Contains(widgetKey);
}

UUserWidget* AComplexHUD::addWidget(FString layerKey, FString widgetKey, FString widgetClassKey, int zIndex, bool hasFocus, bool allowFocusChangeUpdate) {
	UClass* widgetClass = UResourceCache::getWidgetClass(widgetClassKey);
	if (!widgetClass) {
		UDebug::error("Could not add widget " + layerKey + "/" + widgetKey + " becuase widget class for " + widgetClassKey + " could not be found.");
		return nullptr;
	}
	return addWidget(layerKey, widgetKey, widgetClass, zIndex, hasFocus,allowFocusChangeUpdate);
}

UUserWidget* AComplexHUD::addWidget(FString layerKey, FString widgetKey, UClass* widgetClass, int zIndex, bool hasFocus, bool allowFocusChangeUpdate) {
	checkLayer(layerKey, zIndex, true);
	if (widgetLayers[layerKey].widgets.Contains(widgetKey)) {
		removeWidget(layerKey, widgetKey);
	}

	if (!widgetClass) {
		return nullptr;
	}

	UUserWidget* widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), widgetClass, FName(*widgetKey));
	if (!widget) {
		return nullptr;
	}

	ULocalPlayer* localPlayer = GetOwningPlayerController()->GetLocalPlayer();

	int localPlayerCount = UControllerTools::getLocalPlayerCount(GetWorld());
	int localPlayerIndex = UControllerTools::getLocalPlayerIndex(GetOwningPlayerController());
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport){
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	if (UGameViewportSubsystem* subsystem = UGameViewportSubsystem::Get(GetWorld())){
		FGameViewportWidgetSlot slot;

		if (localPlayerCount == 2) {
			if (localPlayerIndex == 1) {
				slot.Offsets.Bottom = viewportSize.Y / 2;
			}
			else if (localPlayerIndex == 2) {
				slot.Offsets.Top = viewportSize.Y / 2;
			}
		}

		slot.ZOrder = zIndex;
		subsystem->AddWidget(widget, slot);
	}
	//widget->AddToPlayerScreen(widgetLayers[layerKey].zIndex);



	widgetLayers[layerKey].widgets.Add(widgetKey, widget);
	if (hasFocus) {
		widgetLayers[layerKey].focusableWidgets.Add(widgetKey);
		if (allowFocusChangeUpdate) {
			updateFocus();
		}
	}

	UDebug::print("Added widget " + layerKey + "." + widgetKey + " to HUD " + this->GetName() + " for player " + GetOwningPlayerController()->GetName());

	return widget;
}

void AComplexHUD::removeWidget(FString layerKey, FString widgetKey, bool allowFocusChangeUpdate) {
	if (widgetLayers.Contains(layerKey)) {
		if (widgetLayers[layerKey].widgets.Contains(widgetKey)) {

			if (widgetLayers[layerKey].widgets[widgetKey] == currentFocusedWidget) {
				currentFocusedWidget = nullptr;
				mouseHoveredWidget = nullptr;
				virtualHoverWidget = nullptr;
			}

			if (lastPrompt == widgetLayers[layerKey].widgets[widgetKey]) {
				lastPrompt = nullptr;
			}
			if (lastVirtualInput == widgetLayers[layerKey].widgets[widgetKey]) {
				lastVirtualInput = nullptr;
			}

			if (UDebugTextWidget::instance == widgetLayers[layerKey].widgets[widgetKey]) {
				UDebugTextWidget::instance->dead = true;
				UDebugTextWidget::instance = nullptr;
			}
			widgetLayers[layerKey].widgets[widgetKey]->RemoveFromParent();
			widgetLayers[layerKey].widgets.Remove(widgetKey);

			if (widgetLayers[layerKey].focusableWidgets.Contains(widgetKey)) {
				widgetLayers[layerKey].focusableWidgets.Remove(widgetKey);
				if (allowFocusChangeUpdate) {
					updateFocus();
				}
			}

		}
	}
}

void AComplexHUD::removeWidget(UUserWidget* widget, bool allowFocusChangeUpdate) {
	FString layerKey, widgetKey;
	for (auto& widgetLayer : widgetLayers) {
		for (auto& widgetEntry : widgetLayer.Value.widgets) {
			if (widgetEntry.Value == widget) {
				layerKey = widgetLayer.Key;
				widgetKey = widgetEntry.Key;
			}
		}
	}
	removeWidget(layerKey, widgetKey);
}

void AComplexHUD::checkWidget(FString layerKey, FString widgetKey, FString widgetClassKey, int zIndex, bool hasFocus, bool shouldHave) {
	if (shouldHave) {
		if (!hasWidget(layerKey, widgetKey)) {
			addWidget(layerKey, widgetKey, widgetClassKey, zIndex, hasFocus);
		}
	}
	else {
		if (hasWidget(layerKey, widgetKey)) {
			removeWidget(layerKey, widgetKey);
		}
	}
}

UUserWidget* AComplexHUD::getWidget(FString layerKey, FString widgetKey) {
	return widgetLayers.Contains(layerKey) && widgetLayers[layerKey].widgets.Contains(widgetKey) ? widgetLayers[layerKey].widgets[widgetKey] : nullptr;
}

UUserWidget* AComplexHUD::getWidget(UClass* targetClass) {
	for (auto& widgetLayer : widgetLayers) {
		for (auto& widgetEntry : widgetLayer.Value.widgets) {
			if (widgetEntry.Value->IsA( targetClass )) {
				return widgetEntry.Value;
			}
		}
	}
	return nullptr;
}


void AComplexHUD::applyWidgetList(FString layerKey, FString widgetListKey, int zIndex, bool allowFocusChangeUpdate) {
	removeLayer(layerKey);
	addLayer(layerKey, zIndex);

	FWidgetList widgetList = UResourceCache::getWidgetList(widgetListKey);
	for (FWidgetListEntry& widget : widgetList.widgets) {
		addWidget(layerKey, widget.key, widget.widgetClass, zIndex, widget.focusable, allowFocusChangeUpdate);
	}
}

FString AComplexHUD::getDefaultWidgetListKey() {
	return "PawnHUD";
}

UUserWidget* AComplexHUD::prompt(FString widgetKey) {
	if (hasWidget("Prompt", widgetKey)) {
		return lastPrompt;
	}
	lastPrompt = addWidget("Prompt",widgetKey,widgetKey,999,true);
	return lastPrompt;
}

void AComplexHUD::removePrompt(FString widgetKey) {
	if (hasWidget("Prompt", widgetKey)) {
		removeLayer("Prompt");
	}
}

UUserWidget* AComplexHUD::openVirtualInput(FString widgetKey) {
	if (hasWidget("VirtualInput", widgetKey)) {
		return lastVirtualInput;
	}
	lastVirtualInput = addWidget("VirtualInput", widgetKey, widgetKey, 1000, true);
	return lastVirtualInput;
}

void AComplexHUD::closeVirtualInput(FString widgetKey) {
	if (hasWidget("VirtualInput", widgetKey)) {
		removeLayer("VirtualInput");
	}
}


void AComplexHUD::setState(FString newState) {
	currentState = newState;
	applyState();
}

void AComplexHUD::applyState() {
	removeAllLayers(false);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	if (gameState->gameStage == EComplexGameStage::MAIN_MENU) {
		applyWidgetList(LAYER_DEFAULT, "MainMenu", 0, false);
	}
	else if (gameState->preGame) {
		if (currentState.Equals(STATE_PAUSED)) {
			applyWidgetList(LAYER_DEFAULT, "Paused", 0, false);
		}
		else {
			applyWidgetList(LAYER_DEFAULT, "PreGame", 0, false);
		}
	}
	else if (gameState->gameOver) {
		if (currentState.Equals(STATE_PAUSED)) {
			applyWidgetList(LAYER_DEFAULT, "Paused", 0, false);
		}
		else {
			applyWidgetList(LAYER_DEFAULT, "EndGame", 0, false);
		}
	}
	else {
		if (currentState.Equals(STATE_DEFAULT)) {
			applyWidgetList(LAYER_DEFAULT, getDefaultWidgetListKey(), 0, false);
		}
		else if (currentState.Equals(STATE_PAUSED)) {
			applyWidgetList(LAYER_DEFAULT, "Paused", 0, false);
		}
	}

	initialized = true;
	addWidget("Debug","Debug","DebugWidget", 9999, false, false);
	updateFocus();
}

void AComplexHUD::handlePossess(APawn* pawn) {
	applyState();
}

void AComplexHUD::handleUnPossess(APawn* pawn) {
	applyState();
}

void AComplexHUD::handleGamepadUseSwitch(bool usingGamepad) {
	//UDebug::print("Input type switched!");
	mouseHoveredWidget = nullptr;
	virtualHoverWidget = nullptr;
	for (auto& widgetLayer : widgetLayers) {
		for (auto& widget : widgetLayer.Value.widgets) {
			UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(widget.Value);
			if (complexWidget) {
				complexWidget->handleGamePadUseChange(usingGamepad);
			}
		}
	}
	updateFocus();
}


AComplexHUD* AComplexHUD::get(APawn* pawn) {
	if (!pawn) {
		return nullptr;
	}
	return get(Cast<APlayerController>(pawn->Controller));
}

AComplexHUD* AComplexHUD::get(APlayerController* controller) {
	if (!controller) {
		return nullptr;
	}
	return Cast< AComplexHUD >(controller->GetHUD());
}

void AComplexHUD::Possess(APawn* pawn) {
	AComplexHUD* hud = get(pawn);
	if (hud) {
		hud->handlePossess(pawn);
	}
}

void AComplexHUD::UnPossess(APawn* pawn) {
	AComplexHUD* hud = get(pawn);
	if (hud) {
		hud->handleUnPossess(pawn);
	}
}

void AComplexHUD::updateFocus() {

	interactableCache.Empty();

	UUserWidget* focusedWidget = nullptr;
	int focusedZIndex = 0;
	mouseHoveredWidget = nullptr;

	for (auto& widgetLayer : widgetLayers) {
		if (focusedWidget != nullptr && focusedZIndex > widgetLayer.Value.zIndex) {
			continue;
		}
		for (FString focusableWidgetKey : widgetLayer.Value.focusableWidgets) {
			focusedWidget = widgetLayer.Value.widgets[focusableWidgetKey];
			focusedZIndex = widgetLayer.Value.zIndex;
		}
	}

	AComplexPlayerController* playerController = Cast< AComplexPlayerController >(PlayerOwner);
	if (!playerController) {
		return;
	}

	bool hasFocusedWidget = focusedWidget != nullptr;
	if (hasFocusedWidget != playerController->uiFocused) {
		playerController->uiFocused = hasFocusedWidget;
		playerController->rebindInputs();
	}
	playerController->updateShowCursor();
	currentFocusedWidget = focusedWidget;
	if (playerController->usingGamepad && currentFocusedWidget && currentFocusedWidget->IsA(UComplexUserWidget::StaticClass())) {
		UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(currentFocusedWidget);
		if (complexWidget->defaultGamePadFocus) {
			setVirtualFocus(complexWidget->defaultGamePadFocus);
		}
	}
}

void AComplexHUD::setVirtualFocus(UUserWidget* widget) {
	mouseHoveredWidget = nullptr;
	virtualHoverWidget = widget;
}

void AComplexHUD::rebindInputs(UInputComponent* inputComponent) {
	TArray< FString > inputNames;
	inputNames.Add("UIMoveDown");
	inputNames.Add("UIMoveUp");
	inputNames.Add("UIMoveLeft");
	inputNames.Add("UIMoveRight");
	inputNames.Add("UIMoveDownAlt");
	inputNames.Add("UIMoveUpAlt");
	inputNames.Add("UIMoveLeftAlt");
	inputNames.Add("UIMoveRightAlt");
	inputNames.Add("UISelect");
	inputNames.Add("UIBack");

	for (FString inputName : inputNames) {
		inputComponent->BindAction<FAnyKeyDelegate>(FName(*inputName), IE_Pressed, this, &AComplexHUD::pressAnyKey, inputName);
		inputComponent->BindAction<FAnyKeyDelegate>(FName(*inputName), IE_Released, this, &AComplexHUD::releaseAnyKey, inputName);
	}
}

void AComplexHUD::pressAnyKey(FString input) {
	UDebug::print("Pressed " + input);
}

void AComplexHUD::releaseAnyKey(FString input) {
	UDebug::print("Released " + input);
}