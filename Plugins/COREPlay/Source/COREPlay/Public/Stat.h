#pragma once

#include "CoreMinimal.h"

#include "Stat.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FStat {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		int value = 1;

	UPROPERTY(EditAnywhere)
		int max = 1;

	UPROPERTY(EditAnywhere)
		float regenRate = 0.0f;

	UPROPERTY()
		float regenTime = 0.0f;

};