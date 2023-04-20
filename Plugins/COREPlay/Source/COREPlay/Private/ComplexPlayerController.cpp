#include "ComplexPlayerController.h"
#include "ComplexGameState.h"
#include "ComplexGameInstance.h"
#include "ComplexHUD.h"
#include "ControllerTools.h"
#include "Debug.h"

void AComplexPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	rebindInputs();
}

void AComplexPlayerController::togglePause() {
    if (paused) {
        unpause();
    }
    else {
        pause();
    }
}


void AComplexPlayerController::pause() {
    AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
    if (gameState->gameStage == EComplexGameStage::MAIN_MENU) {
        return;
    }

    paused = true;

    AComplexHUD* hud = AComplexHUD::get(this);
    if (hud) {
        hud->setState(AComplexHUD::STATE_PAUSED);
    }
    if (gameState) {
        gameState->startPauseChange();
    }
}

void AComplexPlayerController::unpause() {
    paused = false;

    AComplexHUD* hud = AComplexHUD::get(this);
    if (hud) {
        hud->setState(AComplexHUD::STATE_DEFAULT);
    }

    AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
    if (gameState) {
        gameState->startPauseChange();
    }
}

void AComplexPlayerController::quit() {

    AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
    if (gameState) {
        if (UControllerTools::isPlayerOne(this)) {
            for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
                AComplexPlayerController* playerController = Cast< AComplexPlayerController >(Iterator->Get());
                if (playerController && playerController->IsLocalController()) {
                    playerController->sendLeaveNotification();
                }
            }
        }
        else {
            sendLeaveNotification();
        }
    }


    UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

void AComplexPlayerController::rebindInputs() {
	if (InputComponent) {
		InputComponent->ClearActionBindings();
		InputComponent->BindAction("Pause", IE_Pressed, this, &AComplexPlayerController::togglePause);

		if (uiFocused) {
			bindUIInputs();
		}
		
		rebindPawnInputs();
	}
}

void AComplexPlayerController::bindUIInputs() {
	AComplexHUD* hud = AComplexHUD::get(this);
	if (hud) {
        hud->rebindInputs(InputComponent);
	}
}

void AComplexPlayerController::rebindPawnInputs() {
    ACreature* creature = Cast< ACreature >(GetPawn());
    if (creature) {
        creature->rebindInput();
        creature->rebindInput(uiFocused);
    }
}

void AComplexPlayerController::updateShowCursor(){

    if (!UControllerTools::isPlayerOne(this)) {
        return;
    }

    bool showCursor = !usingGamepad && (uiFocused || gameShowCursor);

    if (!showCursor) {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = showCursor;

        /*ULocalPlayer* localPlayer = GetLocalPlayer();
        UGameViewportClient* gameViewportClient = localPlayer->ViewportClient;
        if (!gameViewportClient->GetGameViewportWidget()->HasUserFocus(0)) {
            gameViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);
            gameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
            FSlateApplication::Get().ClearKeyboardFocus();
        }

        FSlateApplication::Get().ClearUserFocus(0);
        FSlateApplication::Get().SetUserFocusToGameViewport(0);*/
    }
    else {
        FInputModeGameAndUI inputMode = FInputModeGameAndUI();
        inputMode.SetHideCursorDuringCapture(false);
        inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
        SetInputMode(inputMode);

        bShowMouseCursor = showCursor;
    }
}

void AComplexPlayerController::OnPossess(APawn* InPawn) {
    Super::OnPossess(InPawn);
    if (InPawn && !InPawn->IsA(ACreature::StaticClass())) {
        AComplexHUD::Possess(InPawn);
    }
}


void AComplexPlayerController::pauseController(APlayerController* controller) {
    AComplexPlayerController* complexController = Cast< AComplexPlayerController >(controller);
    if (complexController) {
        complexController->pause();
    }
}

void AComplexPlayerController::unpauseController(APlayerController* controller) {
    AComplexPlayerController* complexController = Cast< AComplexPlayerController >(controller);
    if (complexController) {
        complexController->unpause();
    }
}

void AComplexPlayerController::quitController(APlayerController* controller) {
    AComplexPlayerController* complexController = Cast< AComplexPlayerController >(controller);
    if (complexController) {
        complexController->quit();
    }
}

void AComplexPlayerController::requestProfileInfo_Implementation() {

    if (UUserManager::instance == nullptr) {
        return;
    }
    UUserProfile* userProfile = UUserManager::instance->getProfile(this);
    if (userProfile != nullptr) {
        receiveProfileInfo(userProfile->username);
    }
}

void AComplexPlayerController::receiveProfileInfo_Implementation(const FString& profileName) {
    UPlayerRecord* player = UPlayerRecord::get(this);
    if (player) {
        if (!player->displayName.Equals(profileName)) {
            player->displayName = profileName;
            AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
            if (gameState) {
                gameState->sendMessage(player->displayName + " has joined", FColor::Silver, "");
            }
        }

        player->profileIdentified = true;
    }
}

void AComplexPlayerController::sendLeaveNotification_Implementation() {
    AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
    if (gameState) {
        gameState->handlePlayerLeave(this);
    }
}

