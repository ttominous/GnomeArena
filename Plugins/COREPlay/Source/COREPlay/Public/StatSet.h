#pragma once

#include "CoreMinimal.h"
#include "Stat.h"

#include "StatSet.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FStatSet {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FStat > stats;

	TMap<FString, int> statsCache;

	bool hasStat(FString key);
	FStat& get(FString key);

};