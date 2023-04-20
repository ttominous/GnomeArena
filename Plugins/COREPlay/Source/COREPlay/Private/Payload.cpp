#include "Payload.h"

FString FPayload::TYPE_ANIM_PING = "AnimPing";
FString FPayload::TYPE_ANIM_END = "AnimEnd";
FString FPayload::TYPE_JUMP = "Jump";
FString FPayload::TYPE_LAUNCH = "Launch";
FString FPayload::TYPE_FREEZE_ANIM = "FreezeAnim";
FString FPayload::TYPE_PLAY_ANIM = "PlayAnim";
FString FPayload::TYPE_PRESS_INPUT = "PressInput";
FString FPayload::TYPE_RELEASE_INPUT = "ReleaseInput";
FString FPayload::TYPE_SWITCH_HANDS = "SwitchHands";
FString FPayload::TYPE_START_ACTION = "StartAction";
FString FPayload::TYPE_STOP_ACTION = "StopAction";
FString FPayload::TYPE_BODY_CHANGE = "BodyChange";
FString FPayload::TYPE_REQUEST_PLAYER_CONTROL = "RequestPlayerControl";

FPayload::FPayload(FString type) {
	setString("type", type);
}

void FPayload::setNumeric(FString key, double value) {
	int previousEntry = numericKeys.Find(key);
	if (previousEntry >= 0) {
		numericValues[previousEntry] = value;
	}
	else {
		numericKeys.Add(key);
		numericValues.Add(value);
	}
}

double FPayload::getNumeric(FString key, double defaultValue) {
	int previousEntry = numericKeys.Find(key);
	if (previousEntry >= 0) {
		return numericValues[previousEntry];
	}
	return defaultValue;
}

bool FPayload::hasNumeric(FString key) {
	return numericKeys.Contains(key);
}

void FPayload::setString(FString key, FString value) {
	int previousEntry = stringKeys.Find(key);
	if (previousEntry >= 0) {
		stringValues[previousEntry] = value;
	}
	else {
		stringKeys.Add(key);
		stringValues.Add(value);
	}
}

FString FPayload::getString(FString key, FString defaultValue) {
	int previousEntry = stringKeys.Find(key);
	if (previousEntry >= 0) {
		return stringValues[previousEntry];
	}
	return defaultValue;
}

bool FPayload::hasString(FString key) {
	return stringKeys.Contains(key);
}


void FPayload::setBool(FString key, bool value) {
	int previousEntry = boolKeys.Find(key);
	if (previousEntry >= 0) {
		boolValues[previousEntry] = value;
	}
	else {
		boolKeys.Add(key);
		boolValues.Add(value);
	}
}

bool FPayload::getBool(FString key, bool defaultValue) {
	int previousEntry = boolKeys.Find(key);
	if (previousEntry >= 0) {
		return boolValues[previousEntry];
	}
	return defaultValue;
}

bool FPayload::hasBool(FString key) {
	return boolKeys.Contains(key);
}

void FPayload::setVector(FString key, FVector vector) {
	FString xKey = key + ".X";
	FString yKey = key + ".Y";
	FString zKey = key + ".Z";

	setNumeric(xKey, vector.X);
	setNumeric(yKey, vector.Y);
	setNumeric(zKey, vector.Z);
}

void FPayload::setVector(FString key, FColor color) {
	FString xKey = key + ".X";
	FString yKey = key + ".Y";
	FString zKey = key + ".Z";

	setNumeric(xKey, (float)color.R / 255.0f);
	setNumeric(yKey, (float)color.G / 255.0f);
	setNumeric(zKey, (float)color.B / 255.0f);
}

FVector FPayload::getVector(FString key, FVector defaultValue) {
	FString xKey = key + ".X";
	FString yKey = key + ".Y";
	FString zKey = key + ".Z";

	return FVector(getNumeric(xKey, defaultValue.X), getNumeric(yKey, defaultValue.Y), getNumeric(zKey, defaultValue.Z));
}

void FPayload::setVector2D(FString key, FVector2D vector) {
	FString xKey = key + ".X";
	FString yKey = key + ".Y";

	setNumeric(xKey, vector.X);
	setNumeric(yKey, vector.Y);
}

FVector2D FPayload::getVector2D(FString key, FVector2D defaultValue) {
	FString xKey = key + ".X";
	FString yKey = key + ".Y";

	return FVector2D(getNumeric(xKey, defaultValue.X), getNumeric(yKey, defaultValue.Y));
}


void FPayload::setRotator(FString key, FRotator rotator) {
	FString xKey = key + ".Pitch";
	FString yKey = key + ".Yaw";
	FString zKey = key + ".Roll";

	setNumeric(xKey, rotator.Pitch);
	setNumeric(yKey, rotator.Yaw);
	setNumeric(zKey, rotator.Roll);
}

FRotator FPayload::getRotator(FString key, FRotator defaultValue) {
	FString xKey = key + ".Pitch";
	FString yKey = key + ".Yaw";
	FString zKey = key + ".Roll";

	return FRotator(getNumeric(xKey, defaultValue.Pitch), getNumeric(yKey, defaultValue.Yaw), getNumeric(zKey, defaultValue.Roll));
}


FPayload FPayload::pressInput(FString inputName) {
	FPayload result;
	result.setString("type", FPayload::TYPE_PRESS_INPUT);
	result.setString("inputName", inputName);
	return result;
}

FPayload FPayload::releaseInput(FString inputName) {
	FPayload result;
	result.setString("type", FPayload::TYPE_RELEASE_INPUT);
	result.setString("inputName", inputName);
	return result;
}

FPayload FPayload::playAnimSequence(FString key, float rate, float startTime, float blendIn, float blendOut, int loops, FString slot) {
	FPayload result;
	result.setString("type", FPayload::TYPE_PLAY_ANIM);
	result.setString("key", key);
	result.setString("slot", slot);
	result.setNumeric("rate", rate);
	result.setNumeric("startTime", startTime);
	result.setNumeric("blendIn", blendIn);
	result.setNumeric("blendOut", blendOut);
	result.setNumeric("loops", loops);
	return result;
}

FPayload FPayload::startAction(FString key) {
	FPayload result;
	result.setString("type", FPayload::TYPE_START_ACTION);
	result.setString("actionKey", key);
	return result;
}