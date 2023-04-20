#pragma once

#include "CoreMinimal.h"
#include "SpawnOption.h"

#include "TeamSpec.generated.h"

class UMindComponent;

USTRUCT()
struct COREPLAY_API FTeamSpec {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnyWhere)
		FString displayName;

	UPROPERTY(EditAnyWhere)
		FColor color;

	UPROPERTY(EditAnyWhere)
		TArray< FSpawnOption > spawnOptions;

	UPROPERTY(EditAnyWhere)
		int maxPlayerCount;

	UPROPERTY(EditAnyWhere)
		float friendlyFireDamage = 1.0f;

	UPROPERTY(EditAnyWhere)
		TSubclassOf< UMindComponent > defaultMindComponentClass = nullptr;

	UPROPERTY(EditAnyWhere)
		TArray< FString > botNames;
};