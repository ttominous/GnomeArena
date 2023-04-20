#include "PlayerRecord.h"
#include "ComplexGameState.h"

#include "GameFramework/SpectatorPawn.h"


APawn* UPlayerRecord::getPawn(bool excludeSpector) {
	
	if (!isBot && controller && controller->GetPawn() != pawn) {
		pawn = nullptr;
	}
	if (pawn == nullptr && controller) {
		pawn = controller->GetPawn();
	}

	if (excludeSpector && pawn && pawn->IsA(ASpectatorPawn::StaticClass())) {
		return nullptr;
	}
	return pawn;
}

UPlayerRecord* UPlayerRecord::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(actor->GetWorld()->GetGameState());
	if (gameState) {
		return gameState->getPlayerRecord(actor);
	}
	return nullptr;
}


UPlayerRecord* UPlayerRecord::get(AController* controller) {
	if (!controller) {
		return nullptr;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(controller->GetWorld()->GetGameState());
	if (gameState) {
		return gameState->getPlayerRecord(controller);
	}
	return nullptr;
}

void UPlayerRecord::resetScore() {
	score = 0;
	kills = 0;
	deaths = 0;
	assists = 0;
}