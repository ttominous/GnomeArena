#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"

#include "ParticleFX.generated.h"


USTRUCT()
struct COREPLAY_API FParticleFX {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UNiagaraSystem* particles = nullptr;

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
		FVector scaleMin = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		FVector scaleMax = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere)
		float chance = 1.0f;

	UPROPERTY(EditAnywhere)
		int minVisualEffectQualityLevel = 0;

};