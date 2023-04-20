#include "GameTools.h"
#include "ComplexGameState.h"
#include "ComplexGameInstance.h"

#include "Kismet/GameplayStatics.h"

bool UGameTools::exitingApplication = false;

void UGameTools::reset() {
	exitingApplication = false;
}

void UGameTools::markExitingApplication() {
	exitingApplication = true;
}


void UGameTools::quitToMainMenu(AActor* actor, bool leaveLobby) {
	moveToMap(actor, UResourceCache::getString("DefaultMainMenuLevel"), leaveLobby);
}

void UGameTools::moveToMap(AActor* actor, FString mapKey, bool leaveLobby) {
	UWorld* world = actor->GetWorld();

	bool inLobby = false;
	bool toMainMenu = mapKey.Equals(UResourceCache::getString("DefaultMainMenuLevel"));

	AComplexGameState* gameState = Cast< AComplexGameState >(world->GetGameState());
	if (gameState) {
		if (leaveLobby && gameState->inLobby) {
			if (world->IsNetMode(ENetMode::NM_Client)) {
				for (FConstPlayerControllerIterator Iterator = world->GetPlayerControllerIterator(); Iterator; ++Iterator) {
					AComplexPlayerController* playerController = Cast< AComplexPlayerController >(Iterator->Get());
					if (playerController && playerController->IsLocalController()) {
						playerController->sendLeaveNotification();
					}
				}
			}
			gameState->leaveLobby();
		}
		gameState->syncToGameInstance();
		inLobby = gameState->inLobby;
	}


	if ( world->IsNetMode(ENetMode::NM_ListenServer) ) {

		if (leaveLobby) {
			UGameplayStatics::OpenLevel(world, FName(*mapKey));
		}
		else if(toMainMenu) {
			APlayerController* controller = UGameplayStatics::GetPlayerController(world, 0);
			FString command = "ServerTravel " + mapKey;
			controller->ConsoleCommand(*command, true);
		}
		else if(inLobby) {
			APlayerController* controller = UGameplayStatics::GetPlayerController(world, 0);
			if (controller) {
				gameState->notifyMatchStart();
				gameState->matchBeginTravelDelay = 3.0f;
			}
		}
		else {
			if (inLobby) {
				mapKey += "?Listen";
			}
			UGameplayStatics::OpenLevel(world, FName(*mapKey));
		}

	}
	else {
		if (inLobby && !leaveLobby) {
			mapKey += "?Listen";
		}
		UGameplayStatics::OpenLevel(world, FName(*mapKey));
	}
}


void UGameTools::openLobby(AActor* actor) {
	UWorld* world = actor->GetWorld();
	AComplexGameState* gameState = Cast< AComplexGameState >(world->GetGameState());
	if (gameState) {
		gameState->openLobby();
	}
	moveToMap(actor, UResourceCache::getString("DefaultMainMenuLevel"), false);
}

void UGameTools::joinIP(UWorld* world, FString ipAddress) {
	FString command = "open " + ipAddress;

	AComplexGameState* gameState = Cast< AComplexGameState >(world->GetGameState());
	if (gameState) {
		gameState->openLobby();
		gameState->syncToGameInstance();
	}

	APlayerController* controller = UGameplayStatics::GetPlayerController(world, 0);
	if (controller){
		gameState->notifyMatchJoining();
		controller->ConsoleCommand(*command, true);
	}
}

FString UGameTools::getGameNameFromWorld(UWorld* world) {
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(world->GetGameInstance());
	if (gameInstance == nullptr) {
		return "";
	}
	return gameInstance->gameName;
}

FString UGameTools::getGameNameFromActor(AActor* actor) {
	return getGameNameFromWorld(actor->GetWorld());
}


bool UGameTools::inLobby(AActor* actor) {
	UWorld* world = actor->GetWorld();
	AComplexGameState* gameState = Cast< AComplexGameState >(world->GetGameState());
	if (gameState) {
		return gameState->inLobby;
	}
	return false;
}