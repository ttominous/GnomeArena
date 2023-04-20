#pragma once

#include "CoreMinimal.h"

#include "SpawnOption.generated.h"


USTRUCT()
struct COREPLAY_API FSpawnOption {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnyWhere)
		FString displayName;

	UPROPERTY(EditAnyWhere)
		TSubclassOf< AActor > actorClass;

	UPROPERTY(EditAnyWhere)
		int maxCountPerTeam = 0;

	UPROPERTY(EditAnyWhere)
		int cost = 0;

};