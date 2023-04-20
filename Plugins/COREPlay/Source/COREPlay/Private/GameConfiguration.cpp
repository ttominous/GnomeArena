#include "GameConfiguration.h"
#include "Debug.h"

void UGameConfigurationInstance::copyFromBase(UGameConfiguration* base) {
	baseConfiguration = base;
	if (!base) {
		return;
	}

	if (base->mapOptions.Num() > 0) {
		for (auto& mapEntry : base->mapOptions) {
			mapKey = mapEntry.Value;
			break;
		}
	}

	displayName = base->displayName;
	description = base->description;
	generalSpawningDelay = base->generalSpawningDelay;
	gameStartDelay = base->gameStartDelay;
	gameEndDelay = base->gameEndDelay;
	killsToWin = base->killsToWin;
	defaultSpawnAreaExtent = base->defaultSpawnAreaExtent;
	debugPathing = base->debugPathing;
	botNames = base->botNames;
	scoreMultiplierDamage = base->scoreMultiplierDamage;
	scoreMultiplierKill = base->scoreMultiplierKill;
	negativeScoreForFriendlyFire = base->negativeScoreForFriendlyFire;
	spawnPlayersAsBots = base->spawnPlayersAsBots;
	autoBotIdlePlayers = base->autoBotIdlePlayers;
	idlePlayerBotTime = base->idlePlayerBotTime;
	pauseStopsTime = base->pauseStopsTime;
}