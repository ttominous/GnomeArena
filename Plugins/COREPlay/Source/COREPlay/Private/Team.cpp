#include "Team.h"
#include "ComplexGameState.h"

int UTeam::getPawnsOfClassCount(UClass* pawnClass) {
	if (pawnClass == nullptr) {
		return 0;
	}

	int total = 0;
	for (UPlayerRecord* player : players) {
		if (player->pawn && player->pawn->IsA(pawnClass)) {
			total++;
		}
	}
	return total;
}

int UTeam::getHumanPlayerCount() {
	int total = 0;
	for (UPlayerRecord* player : players) {
		if (!player->isBot) {
			total++;
		}
	}
	return total;
}

int UTeam::getBotPlayerCount() {
	int total = 0;
	for (UPlayerRecord* player : players) {
		if (player->isBot) {
			total++;
		}
	}
	return total;
}

UTeam* UTeam::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(actor->GetWorld()->GetGameState());
	if (gameState) {
		return gameState->getTeam(actor);
	}
	return nullptr;
}

UTeam* UTeam::get(AController* controller) {
	if (!controller) {
		return nullptr;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(controller->GetWorld()->GetGameState());
	if (gameState) {
		return gameState->getTeam(controller);
	}
	return nullptr;
}

UTeam* UTeam::get(UPlayerRecord* player) {
	if (!player) {
		return nullptr;
	}

	if (player->team) {
		return player->team;
	}

	if (player->pawn) {
		return get(player->pawn);
	}
	if (player->controller) {
		return get(player->controller);
	}
	return nullptr;
}

UTeam* UTeam::get(UWorld* world, FTargetDetail& targetDetail) {
	if (targetDetail.type == ETargetClass::CREATURE) {
		return get( Cast<AActor>(targetDetail.getCreature(world)) );
	}
	return nullptr;
}

bool UTeam::isHostile(UTeam* otherTeam) {
	if (otherTeam == nullptr) {
		return true;
	}
	return otherTeam != this;
}

bool UTeam::isHostile(UTeam* teamA, UTeam* teamB) {
	if (teamA == nullptr) {
		return true;
	}
	return teamA->isHostile(teamB);
}

bool UTeam::nameIsUsed(FString name) {
	for (UPlayerRecord* player : players) {
		if (player->displayName.Equals(name)) {
			return true;
		}
	}
	return false;
}
FString UTeam::getNextBotName() {
	if (botNames.Num() > 0) {
		bool keepTrying = true;
		int level = 1;
		while (keepTrying) {

			TArray< FString > possibleNames;
			for (FString botName : botNames) {
				FString actualName = level == 1 ? botName : botName + " " + FString::FromInt(level);
				if (!nameIsUsed(actualName)) {
					possibleNames.Add(actualName);
				}
			}

			if (possibleNames.Num() > 0) {
				return possibleNames[FMath::RandRange(0,possibleNames.Num()-1)];
			}

			level++;
		}

	}
	return "AI Bot";
}

void UTeam::resetScore() {
	score = 0;
	kills = 0;
	deaths = 0;
	assists = 0;
}