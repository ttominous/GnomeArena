#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "DrawDebugHelpers.h"

class COREPLAY_API UDebug {

public:

	static void log(FString text);
	static void print(FString text);
	static void print(FString text, FColor color);
	static void print(FString text, FColor color, float time);
	static void error(FString text);
	static void netError(UObject* object, FString text);
	static void netError(UWorld* world, FString text);
	static void netPrint(UObject* object, FString text, FColor color = FColor::White, float time = 5.0f);
	static void netPrint(UWorld* world, FString text, FColor color = FColor::White, float time = 5.0f);
	static void tick(FString text);
	static void tick(FString text, FColor color);
	static void netTick(UWorld* world, FString text);
	static void drawSphere(UWorld* world, FVector location, float radius, FColor color, float lifeSpan = -1.0f);
	static void drawBox(UWorld* world, FVector min, FVector max, FColor color, float lineThickness, float lifeSpan = -1.0f);
	static void drawBox(UWorld* world, FBox box, FColor color, float lineThickness, float lifeSpan = -1.0f);
	static void drawBox(UWorld* world, FVector location, FQuat rotation, FVector size, FColor color, float lineThickness, float lifeSpan = -1.0f);
	static void drawLine(UWorld* world, FVector start, FVector end, FColor color, float thickness, float lifeSpan = -1.0f);
	static void drawPoint(UWorld* world, FVector location, float radius, FColor color, float lifeSpan = -1.0f);
	static void drawText(UWorld* world, FVector location, FString text, FColor color, float lifeSpan = -1.0f);
};
