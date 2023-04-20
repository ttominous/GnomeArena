#include "SpawnPoint.h"
#include "Debug.h"

FString ASpawnPoint::getType() {
	return "SpawnPoint";
}

TArray< ASpawnPoint* > ASpawnPoint::getAll() {
	TArray< ASpawnPoint* > results;
	if (APointOfInterest::instancesByType.Contains("SpawnPoint")) {
		for (APointOfInterest* pointOfInterest : APointOfInterest::instancesByType["SpawnPoint"].instances) {
			ASpawnPoint* spawnPoint = Cast< ASpawnPoint >(pointOfInterest);
			if (spawnPoint) {
				results.Add(spawnPoint);
			}
		}
	}
	return results;
}