#include "PointOfInterest.h"
#include "Debug.h"

TArray< APointOfInterest* > APointOfInterest::instances;
TMap< FString, FPointOfInterestList > APointOfInterest::instancesByType;


APointOfInterest::APointOfInterest() {
	PrimaryActorTick.bCanEverTick = false;
}


void APointOfInterest::BeginPlay() {
	Super::BeginPlay();
	if (!instances.Contains(this)) {
		instances.Add(this);
	}

	FString type = getType();
	if (!instancesByType.Contains(type)) {
		instancesByType.Add(type, FPointOfInterestList());
	}
	if (!instancesByType[type].instances.Contains(this)) {
		instancesByType[type].instances.Add(this);
	}
}

void APointOfInterest::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);

	FString type = getType();
	if (instancesByType.Contains(type)) {
		instancesByType[type].instances.Remove(this);
	}
	Super::EndPlay(EndPlayReason);
}

FString APointOfInterest::getType() {
	return "Default";
}