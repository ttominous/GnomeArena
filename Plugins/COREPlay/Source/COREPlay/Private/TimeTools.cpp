#include "TimeTools.h"
#include "Debug.h"

#include "Kismet/GameplayStatics.h"

float UTimeTools::preferredTimeDilation = 1.0f;
bool UTimeTools::paused = false;


FDateTime UTimeTools::getSystemDateTime() {
	int32 year, month, day, dayOfWeek, hour, minute, second, milliSecond;
	FPlatformTime::SystemTime(year, month, dayOfWeek, day, hour, minute, second, milliSecond);
	return FDateTime(year, month, day, hour, minute, second, milliSecond);
}

FString UTimeTools::getSytemDateTimeString() {
	return getSystemDateTime().ToString();
}

void UTimeTools::setTimeDilation(UWorld* world, float timeDialation) {
	UGameplayStatics::SetGlobalTimeDilation(world, timeDialation);
}

void UTimeTools::pauseTime(UWorld* world) {
	setTimeDilation(world, 0.0f);
	paused = true;
}

void UTimeTools::resumeTime(UWorld* world) {
	setTimeDilation(world, preferredTimeDilation);
	paused = false;
}

void UTimeTools::setPreferredTimeDilation(UWorld* world, float newTimeDilation) {
	preferredTimeDilation = newTimeDilation;
	if (!paused) {
		UGameplayStatics::SetGlobalTimeDilation(world, preferredTimeDilation);
	}
}

void UTimeTools::resetTimeDilation(UWorld* world) {
	preferredTimeDilation = 1.0f;
	setTimeDilation(world, preferredTimeDilation);
	paused = false;
}

bool UTimeTools::isPaused(UWorld* world) {
	return UGameplayStatics::GetGlobalTimeDilation(world) <= 0.01f;
}