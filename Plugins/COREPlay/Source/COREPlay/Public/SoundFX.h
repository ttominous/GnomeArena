#pragma once

#include "CoreMinimal.h"

#include "SoundFX.generated.h"



USTRUCT()
struct COREPLAY_API FSoundFX {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		USoundBase* sound = nullptr;

	UPROPERTY(EditAnywhere)
		FVector offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector offsetRangeMin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector offsetRangeMax = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector rotationMin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector rotationMax = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		float volumeMin = 1.0f;

	UPROPERTY(EditAnywhere)
		float volumeMax = 1.0f;

	UPROPERTY(EditAnywhere)
		float pitchMin = 1.0f;

	UPROPERTY(EditAnywhere)
		float pitchMax = 1.0f;

	UPROPERTY(EditAnywhere)
		float startTimeMin = 0.0f;

	UPROPERTY(EditAnywhere)
		float startTimeMax = 0.0f;

	UPROPERTY(EditAnywhere)
		float chance = 1.0f;

};