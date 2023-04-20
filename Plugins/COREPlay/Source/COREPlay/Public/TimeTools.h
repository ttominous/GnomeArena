#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformTime.h"

#include "TimeTools.generated.h"


UCLASS()
class COREPLAY_API UTimeTools : public UObject {

	GENERATED_BODY()

public:

	static float preferredTimeDilation;
	static bool paused;

	static FDateTime getSystemDateTime();
	static FString getSytemDateTimeString();
	static void setTimeDilation(UWorld* world, float timeDialation);
	static void pauseTime(UWorld* world);
	static void resumeTime(UWorld* world);
	static void setPreferredTimeDilation(UWorld* world, float newTimeDilation);
	static void resetTimeDilation(UWorld* world);
	static bool isPaused(UWorld* world);
};