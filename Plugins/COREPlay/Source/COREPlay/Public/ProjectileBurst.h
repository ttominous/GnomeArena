#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileSpec.h"

#include "ProjectileBurst.generated.h"


USTRUCT()
struct COREPLAY_API FProjectileBurst {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<AProjectile> projectileClass = nullptr;

	UPROPERTY(EditAnywhere)
		int minCount = 1;

	UPROPERTY(EditAnywhere)
		int maxCount = 1;

	UPROPERTY(EditAnywhere)
		FVector minLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FVector maxLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FRotator minRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
		FRotator maxRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
		float speedModifier = 1.0f;

	UPROPERTY(EditAnywhere)
		UProjectileSpec* projectileSpec = nullptr;

};