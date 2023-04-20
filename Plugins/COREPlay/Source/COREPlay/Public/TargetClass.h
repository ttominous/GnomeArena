#pragma once

#include "CoreMinimal.h"

#include "TargetClass.generated.h"

UENUM(BlueprintType)
enum class ETargetClass : uint8 {
	UNKNOWN = 0						UMETA(DisplayName = "Unknown"),
	CREATURE = 1					UMETA(DisplayName = "Creature"),
	LOCATION = 2					UMETA(DisplayName = "Location"),
	SCENE_OBJECT = 3				UMETA(DisplayName = "SceneObject"),
	TERRAIN = 4						UMETA(DisplayName = "Terrain"),
	DROP_ITEM = 5					UMETA(DisplayName = "DropItem")
};
