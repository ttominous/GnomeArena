#include "ComplexViewportClient.h"
#include "ComplexGameInstance.h"
#include "ComplexHUD.h"
#include "ComplexUserWidget.h"
#include "ComplexPlayerController.h"
#include "ControllerTools.h"
#include "GameTools.h"
#include "Debug.h"

bool UComplexViewportClient::InputKey(const FInputKeyEventArgs& EventArgs) {

	if (UGameTools::exitingApplication) {
		return false;
	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GameInstance);
	if (gameInstance == nullptr) {
		return Super::InputKey(EventArgs);
	}

	bool usingGamepad = EventArgs.Key.IsGamepadKey();
	ULocalPlayer* localPlayer = gameInstance->getLocalPlayerOrCreateNew(EventArgs.ControllerId, usingGamepad);
	if (localPlayer == nullptr) {
		return false;
	}
	int controllerID = localPlayer->GetControllerId();

	APlayerController* playerController = Cast< APlayerController >(localPlayer->GetPlayerController(gameInstance->GetWorld()));

	if (playerController != nullptr) {
		AComplexHUD* hud = AComplexHUD::get(playerController);
		AComplexPlayerController* complexController = Cast< AComplexPlayerController >(playerController);
		if (complexController) {

			if (EventArgs.ControllerId == 0 && !complexController->verifiedWantsGamepad) {
				if (hud) {
					hud->prompt("WantGamepad");
				}
			}

			if (usingGamepad != complexController->usingGamepad) {
				complexController->usingGamepad = usingGamepad;
				if (hud) {
					hud->handleGamepadUseSwitch(usingGamepad);
				}
			}
			else {
				if (hud && hud->virtualHoverWidget) {

					UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(hud->virtualHoverWidget);
					if (complexWidget) {
						if (EventArgs.Event == EInputEvent::IE_Released) {
							complexWidget->handleInputRelease(EventArgs.Key);
						}
						else if (EventArgs.Event == EInputEvent::IE_Pressed) {
							complexWidget->handleInputPress(EventArgs.Key);
						}
						else if (EventArgs.Event == EInputEvent::IE_Repeat && complexController->GetInputKeyTimeDown(EventArgs.Key) >= 0.5f) {
							complexWidget->handleInputPress(EventArgs.Key);
						}
					}
				}
			}
		}
		return playerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));
	}

	FInputKeyEventArgs outEventArgs(localPlayer->ViewportClient->Viewport, controllerID, EventArgs.Key, EventArgs.Event);

	if (UGameTools::exitingApplication) {
		return false;
	}
	return Super::InputKey(outEventArgs);
}


bool UComplexViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) {
	
	if (UGameTools::exitingApplication) {
		return false;
	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GameInstance);
	if (gameInstance == nullptr) {
		return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	ULocalPlayer* localPlayer = gameInstance->getLocalPlayerOrCreateNew(InputDevice.GetId(), bGamepad);
	if (localPlayer == nullptr) {
		return false;
	}

	APlayerController* playerController = Cast< APlayerController >(localPlayer->GetPlayerController(gameInstance->GetWorld()));

	if (playerController != nullptr) {
		AComplexHUD* hud = AComplexHUD::get(playerController);
		AComplexPlayerController* complexController = Cast< AComplexPlayerController >(playerController);
		if (complexController) {
			if (bGamepad != complexController->usingGamepad) {
				complexController->usingGamepad = bGamepad;
				if (hud) {
					hud->handleGamepadUseSwitch(bGamepad);
				}
			}
		}
		return playerController->InputKey(FInputKeyParams(Key, (double)Delta, DeltaTime, NumSamples, bGamepad, InputDevice));
	}

	return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}