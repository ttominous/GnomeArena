#pragma once

#include "CoreMinimal.h"
#include "DamageClass.h"
#include "TargetDetail.h"

#include "DamageDetails.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FDamageDetail {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		EDamageClass type;

	UPROPERTY(EditAnywhere)
		int amount = 0;

	UPROPERTY(EditAnywhere)
		bool critical = false;
};


USTRUCT(BlueprintType)
struct COREPLAY_API FDamageDetails {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FTargetDetail attacker;

	UPROPERTY(EditAnywhere)
		FTargetDetail victim;

	UPROPERTY(EditAnywhere)
		TArray< FDamageDetail > damages;

	UPROPERTY(EditAnywhere)
		FString description;

	UPROPERTY(EditAnywhere)
		FString iconKey;

	UPROPERTY(EditAnywhere)
		FString effectKey;

	UPROPERTY(EditAnywhere)
		float force = 0.0f;

	UPROPERTY(EditAnywhere)
		float stagger = 0.0f;

	UPROPERTY(EditAnywhere)
		bool forceFaceAttackerAim = false;

	UPROPERTY(EditAnywhere)
		bool forceOnlyUsesXY = false;

	FDamageDetails add(FDamageDetails& other);
};