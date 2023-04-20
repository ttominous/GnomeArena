#include "ControllerTools.h"
#include "ComplexHUD.h"

int UControllerTools::getLocalPlayerCount(UWorld* world) {
	int localPlayerCount = 0;
	for (FConstPlayerControllerIterator Iterator = world->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController && playerController->IsLocalController()) {
			localPlayerCount++;
		}
	}
	return localPlayerCount;
}

bool UControllerTools::isPlayerOne(APlayerController* playerController) {
	return getLocalPlayerIndex(playerController) == 0;
}

bool UControllerTools::hasLocalMultiplayer(UWorld* world) {
	return getLocalPlayerCount(world) > 1;
}

int UControllerTools::getLocalPlayerIndex(APlayerController* playerController) {
	if (playerController != nullptr) {
		int playerIndex = 0;
		for (FConstPlayerControllerIterator Iterator = playerController->GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
			APlayerController* otherController = Cast< APlayerController >(Iterator->Get());
			if (otherController && otherController->IsLocalController()) {
				if (otherController == playerController) {
					return playerIndex;
				}
				playerIndex++;
			}
		}
	}
	return -1;
}

bool UControllerTools::isGamePad(APlayerController* playerController) {
	return false;
}

bool UControllerTools::isHoveringUI(APlayerController* playerController) {
	if (playerController == nullptr) {
		return false;
	}
	AComplexHUD* hud = AComplexHUD::get(playerController);
	return hud != nullptr ? hud->mouseHoveredWidget != nullptr : false;
}