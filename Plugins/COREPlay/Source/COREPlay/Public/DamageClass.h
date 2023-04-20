#pragma once

#include "CoreMinimal.h"

#include "DamageClass.generated.h"

UENUM(BlueprintType)
enum class EDamageClass : uint8 {
	AMBIENT = 0						UMETA(DisplayName = "Ambient"),
	BLUNT = 1						UMETA(DisplayName = "Blunt"),
	PIERCE = 2						UMETA(DisplayName = "Pierce"),
	SLASH = 3						UMETA(DisplayName = "Slash"),
	POISON = 4						UMETA(DisplayName = "Poison"),
	BURN = 5						UMETA(DisplayName = "Burn"),
	COLD = 6						UMETA(DisplayName = "Cold"),
	HOLY = 7						UMETA(DisplayName = "Holy"),
	CORRUPTION = 8					UMETA(DisplayName = "Corruption"),
	CHOP = 9						UMETA(DisplayName = "Chop"),
	MINE = 10						UMETA(DisplayName = "Mine")
};
