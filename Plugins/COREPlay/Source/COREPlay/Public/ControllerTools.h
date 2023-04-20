#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ControllerTools.generated.h"

UCLASS()
class COREPLAY_API UControllerTools : public UObject {

	GENERATED_BODY()

public:

	static int getLocalPlayerCount(UWorld* world);
	static bool isPlayerOne(APlayerController* playerController);
	static bool hasLocalMultiplayer(UWorld* world);
	static int getLocalPlayerIndex(APlayerController* playerController);
	static bool isGamePad(APlayerController* playerController);
	static bool isHoveringUI(APlayerController* playerController);
};
