#pragma once

#include "CoreMinimal.h"

#include "SlotAnimState.generated.h"

UENUM(BlueprintType)
enum class ESlotAnimState : uint8 {
	FULL_BODY = 0						UMETA(DisplayName = "FullBody"),
	LIMBS_KEEP_ROTATION = 1				UMETA(DisplayName = "LimbsKeepRotation"),
	SECTION_OF_BODY = 2					UMETA(DisplayName = "SectionOfBody"),
	NONE = 3							UMETA(DisplayName = "None"),
	LIMBS_LOOSE = 4						UMETA(DisplayName = "LimbsLoose")
};
