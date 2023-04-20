#include "ComplexGameMode.h"
#include "ComplexGameState.h"
#include "Creature.h"
#include "ResourceCache.h"
#include "Debug.h"

void AComplexGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (Exiting == nullptr || Exiting->IsA(APlayerController::StaticClass())) {
		return;
	}

	AComplexGameState* gameState = Cast< AComplexGameState > (GetWorld()->GetGameState());
	if (gameState) {
		APlayerController* playerController = Cast< APlayerController >(Exiting);
		gameState->handlePlayerLeave(playerController);
	}
}

void AComplexGameMode::SpawnCreature(const FString& specKey) {
	FVector position = FVector(0,0,100);
	float yaw = 0.0f;

	ACreature* creature = ACreature::spawn(GetWorld(), specKey, position, yaw);
	if (creature) {
		UDebug::print("Spawned " + specKey, FColor::Green);
	}
	else {
		UDebug::error("Failed to spawn " + specKey);
	}
}

void AComplexGameMode::RemoveCreature() {
	
}
