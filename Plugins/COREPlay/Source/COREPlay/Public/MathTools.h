#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

#include "MathTools.generated.h"


UCLASS()
class COREPLAY_API UMathTools : public UObject {

	GENERATED_BODY()

public:

	static float CONST_PI;
	static float CONST_180_DIV_PI;
	static FVector2D CONST_FORWARD_2D;


	static float getAngleDiff(float a, float b, bool alwaysPositive=false);
	static float getAngle(FVector2D direction);
	static float getAngle(float x, float y);
	static float getAngle(float x, float y, float x2, float y2);
	static float getAngle(FVector2D eye, FVector2D target);
	static float getYaw(FVector eye, FVector target);
	static float getPitch(FVector eye, FVector target);
	static FRotator getYawAndPitch(FVector eye, FVector target);
	static float getSafeAngle(float angle);
	static float clampPitch(float value, float range);

	static float lerpAngleByDegrees(float angleA, float angleB, float degrees);
	static float lerpAngleByPerc(float angleA, float angleB, float percentage);
	static float lerpFloatByAmount(float A, float B, float amount);
	static float lerpFloatByPerc(float A, float B, float amount);
	static FVector lerpVectorByAmount(FVector A, FVector B, float amount);
	static FVector lerpVectorByPerc(FVector A, FVector B, float amount);
	static FRotator lerpRotatorByAmount(FRotator A, FRotator B, float amount);
	static FRotator lerpRotatorByPerc(FRotator A, FRotator B, float amount);

	static FVector randomVectorFromRange(float rangeX, float rangeY, float rangeZ);
	static FVector randomVectorFromRange(FVector min, FVector max);
	static FRotator randomRotatorFromRange(float rangePitch, float rangeYaw, float rangeRoll);
	static FRotator randomRotatorFromRange(FRotator min, FRotator max);

	static FVector calculateNormal(FVector A, FVector B, FVector C);

	static void sortMinMax(FIntPoint& A, FIntPoint& B);
};