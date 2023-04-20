#include "MathTools.h"

float UMathTools::CONST_PI = 3.1415926F;
float UMathTools::CONST_180_DIV_PI = 180.0F / UMathTools::CONST_PI;
FVector2D UMathTools::CONST_FORWARD_2D = FVector2D(0.0F, 1.0F);

float UMathTools::getAngleDiff(float a, float b, bool alwaysPositive) {
	a = getSafeAngle(a);
	b = getSafeAngle(b);

	float ccw = 0.0F;
	float cw = 0.0F;

	if (b >= a) {
		ccw = (360.0F - b) + a;
		cw = b - a;
	}
	else {
		ccw = a - b;
		cw = (360.0F - a) + b;
	}

	if (ccw <= cw) {

		if (alwaysPositive) {
			return ccw;
		}
		return -ccw;
	}
	return cw;
}

float UMathTools::getAngle(FVector2D direction) {

	float bottom = direction.Size();
	if (bottom == 0) {
		return 0.0F;
	}

	float dot = FVector2D::DotProduct(direction, CONST_FORWARD_2D);
	float result = -FMath::RadiansToDegrees(FMath::Acos(dot / bottom));

	if (direction.X < 0.0F) {
		result = -result;
	}
	return result;
}

float UMathTools::getAngle(float x, float y) {
	return getAngle(FVector2D(x, y));
}

float UMathTools::getAngle(float x, float y, float x2, float y2) {
	return getAngle(FVector2D(x2 - x, y2 - y));
}

float UMathTools::getAngle(FVector2D eye, FVector2D target) {
	return getAngle(FVector2D(target.X - eye.X, target.Y - eye.Y));
}

float UMathTools::getYaw(FVector eye, FVector target) {
	return getAngle(eye.X, eye.Y, target.X, target.Y);
}

float UMathTools::getPitch(FVector eye, FVector target) {
	float xDiff = target.X - eye.X;
	float yDiff = target.Y - eye.Y;
	float xyDist = FMath::Sqrt(xDiff * xDiff + yDiff * yDiff);
	return getAngle(0, 0, xyDist, target.Z - eye.Z);
}

FRotator UMathTools::getYawAndPitch(FVector eye, FVector target) {
	float yaw = getAngle(eye.X, eye.Y, target.X, target.Y) + 90.0f;
	float xDiff = target.X - eye.X;
	float yDiff = target.Y - eye.Y;
	float xyDist = FMath::Sqrt(xDiff * xDiff + yDiff * yDiff);
	float pitch = getAngle(0, 0, xyDist, target.Z - eye.Z);
	return FRotator(pitch + 90.0f, yaw, 0.0f);
}

float UMathTools::getSafeAngle(float angle) {
	if (angle >= 360.0F) {
		return FMath::Fmod(angle, 360.0F);
	}
	else if (angle < 0.0F) {
		return 360.0F - (FMath::Fmod(-angle, 360.0F));
	}
	return angle;
}

float UMathTools::clampPitch(float value, float range) {
	value = getSafeAngle(value);
	if (value > 180.0f) {
		value = -(360.0f - value);
	}
	return FMath::Clamp(value, -range, range);
}



float UMathTools::lerpAngleByDegrees(float angleA, float angleB, float degrees) {

	angleA = getSafeAngle(angleA);
	angleB = getSafeAngle(angleB);

	float ccw = 0.0F;
	float cw = 0.0F;

	if (angleB >= angleA) {
		ccw = (360.0F - angleB) + angleA;
		cw = angleB - angleA;
	}
	else {
		ccw = angleA - angleB;
		cw = (360.0F - angleA) + angleB;
	}

	if (ccw <= cw) {
		if (degrees > ccw) {
			return angleB;
		}
		else {
			return getSafeAngle(angleA - degrees);
		}
	}
	else {
		if (degrees > cw) {
			return angleB;
		}
		else {
			return getSafeAngle(angleA + degrees);
		}
	}
}

float UMathTools::lerpAngleByPerc(float angleA, float angleB, float percentage) {

	angleA = getSafeAngle(angleA);
	angleB = getSafeAngle(angleB);

	float ccw = 0.0F;
	float cw = 0.0F;

	if (angleB >= angleA) {
		ccw = (360.0F - angleB) + angleA;
		cw = angleB - angleA;
	}
	else {
		ccw = angleA - angleB;
		cw = (360.0F - angleA) + angleB;
	}

	if (ccw <= cw) {
		return getSafeAngle(angleA - ccw * percentage);
	}
	else {
		return getSafeAngle(angleA + cw * percentage);
	}
}



float UMathTools::lerpFloatByAmount(float A, float B, float amount) {

	if (B > A) {
		A += amount;
		if (A > B) {
			A = B;
		}
	}
	else if (B < A) {
		A -= amount;
		if (A < B) {
			A = B;
		}
	}

	return A;
}

float UMathTools::lerpFloatByPerc(float A, float B, float amount) {
	if (amount > 1.0f) {
		amount = 1.0f;
	}
	return (B - A) * amount + A;
}

FVector UMathTools::lerpVectorByAmount(FVector A, FVector B, float amount) {
	return FVector( lerpFloatByAmount(A.X,B.X,amount), lerpFloatByAmount(A.Y, B.Y, amount), lerpFloatByAmount(A.Z, B.Z, amount));
}

FVector UMathTools::lerpVectorByPerc(FVector A, FVector B, float amount) {
	amount = FMath::Clamp(amount, 0.0f, 1.0f);
	return (B - A) * amount + A;
}

FRotator UMathTools::lerpRotatorByAmount(FRotator A, FRotator B, float amount) {
	return FRotator(lerpAngleByDegrees(A.Pitch, B.Pitch, amount), lerpAngleByDegrees(A.Yaw, B.Yaw, amount), lerpAngleByDegrees(A.Roll, B.Roll, amount));
}

FRotator UMathTools::lerpRotatorByPerc(FRotator A, FRotator B, float amount) {
	amount = FMath::Clamp(amount, 0.0f, 1.0f);
	return FRotator(lerpAngleByPerc(A.Pitch, B.Pitch, amount), lerpAngleByPerc(A.Yaw, B.Yaw, amount), lerpAngleByPerc(A.Roll, B.Roll, amount));
}


FVector UMathTools::randomVectorFromRange(float rangeX, float rangeY, float rangeZ) {
	return FVector(FMath::RandRange(-rangeX, rangeX), FMath::RandRange(-rangeY, rangeY), FMath::RandRange(-rangeZ, rangeZ));
}

FVector UMathTools::randomVectorFromRange(FVector min, FVector max) {
	return FVector( FMath::RandRange(min.X, max.X), FMath::RandRange(min.Y, max.Y), FMath::RandRange(min.Z, max.Z));
}


FRotator UMathTools::randomRotatorFromRange(float rangePitch, float rangeYaw, float rangeRoll) {
	return FRotator(FMath::RandRange(-rangePitch, rangePitch), FMath::RandRange(-rangeYaw, rangeYaw), FMath::RandRange(-rangeRoll, rangeRoll));
}

FRotator UMathTools::randomRotatorFromRange(FRotator min, FRotator max) {
	return FRotator(FMath::RandRange(min.Pitch, max.Pitch), FMath::RandRange(min.Yaw, max.Yaw), FMath::RandRange(min.Roll, max.Roll));
}

FVector UMathTools::calculateNormal(FVector A, FVector B, FVector C) {
	FVector normal = FVector::CrossProduct((C - A), (B - A));
	normal.Normalize();
	return normal;
}

void UMathTools::sortMinMax(FIntPoint& A, FIntPoint& B) {
	FIntPoint min(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
	FIntPoint max(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));
	A = min;
	B = max;
}