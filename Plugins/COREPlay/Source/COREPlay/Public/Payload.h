#pragma once

#include "CoreMinimal.h"

#include "Payload.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FPayload {

	GENERATED_BODY()

public:

	static FString TYPE_ANIM_PING;
	static FString TYPE_ANIM_END;
	static FString TYPE_JUMP;
	static FString TYPE_LAUNCH;
	static FString TYPE_FREEZE_ANIM;
	static FString TYPE_PLAY_ANIM;
	static FString TYPE_PRESS_INPUT;
	static FString TYPE_RELEASE_INPUT;
	static FString TYPE_SWITCH_HANDS;
	static FString TYPE_START_ACTION;
	static FString TYPE_STOP_ACTION;
	static FString TYPE_BODY_CHANGE;
	static FString TYPE_REQUEST_PLAYER_CONTROL;

	UPROPERTY(EditAnywhere)
		TArray< FString > numericKeys;

	UPROPERTY(EditAnywhere)
		TArray< double > numericValues;


	UPROPERTY(EditAnywhere)
		TArray< FString > stringKeys;

	UPROPERTY(EditAnywhere)
		TArray< FString > stringValues;


	UPROPERTY(EditAnywhere)
		TArray< FString > boolKeys;

	UPROPERTY(EditAnywhere)
		TArray< bool > boolValues;

	FPayload() {}
	FPayload(FString type);

	void setNumeric(FString key, double value);
	double getNumeric(FString key, double defaultValue = 0.0);
	bool hasNumeric(FString key);

	void setString(FString key, FString value);
	FString getString(FString key, FString defaultValue = "");
	bool hasString(FString key);

	void setBool(FString key, bool value);
	bool getBool(FString key, bool defaultValue = false);
	bool hasBool(FString key);

	void setVector(FString key, FVector vector);
	void setVector(FString key, FColor color);
	FVector getVector(FString key, FVector defaultValue);

	void setVector2D(FString key, FVector2D vector);
	FVector2D getVector2D(FString key, FVector2D defaultValue);

	void setRotator(FString key, FRotator rotator);
	FRotator getRotator(FString key, FRotator defaultValue);

	static FPayload pressInput(FString inputName);
	static FPayload releaseInput(FString inputName);
	static FPayload playAnimSequence(FString key, float rate = 1.0f, float startTime = 0.0f, float blendIn = 0.05f, float blendOut = 0.2f, int loops = 1, FString slot = "DefaultSlot");
	static FPayload startAction(FString key);


};