#pragma once

#include "CoreMinimal.h"

#include "ComplexGameStage.generated.h"

UENUM(BlueprintType)
enum class EComplexGameStage : uint8 {
    MAIN_MENU = 0                   UMETA(DisplayName = "MainMenu"),
    IN_GAME = 1                     UMETA(DisplayName = "InGame")
};