#pragma once

#include "CoreMinimal.h"
#include "PointOfInterest.h"

#include "SpawnPoint.generated.h"

UCLASS()
class COREPLAY_API ASpawnPoint : public APointOfInterest {

	GENERATED_BODY()

public:

	virtual FString getType() override;

	static TArray< ASpawnPoint* > getAll();

};