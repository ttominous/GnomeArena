#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PointOfInterest.generated.h"

USTRUCT()
struct COREPLAY_API FPointOfInterestList {
	GENERATED_BODY()

public:

	TArray< APointOfInterest* > instances;
};

UCLASS()
class COREPLAY_API APointOfInterest : public AActor {

	GENERATED_BODY()

public:

	static TArray< APointOfInterest* > instances;
	static TMap< FString, FPointOfInterestList > instancesByType;

	APointOfInterest();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FString getType();

};