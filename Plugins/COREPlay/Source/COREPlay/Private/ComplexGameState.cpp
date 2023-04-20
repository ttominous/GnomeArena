#include "ComplexGameState.h"
#include "ComplexGameInstance.h"
#include "ComplexGameMode.h"
#include "ComplexPlayerController.h"
#include "UserManager.h"
#include "MindComponent.h"
#include "ComplexHud.h"
#include "PlayerHUD.h"
#include "Creature.h"
#include "SceneObject.h"
#include "DropItem.h"
#include "SpawnPoint.h"
#include "ControllerTools.h"
#include "TimeTools.h"
#include "MathTools.h"
#include "GameTools.h"
#include "HTTPTools.h"
#include "Debug.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"


AComplexGameState::AComplexGameState() {
	bReplicateUsingRegisteredSubObjectList = true;
}

void AComplexGameState::OnRep_RefreshHUD() {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = AComplexHUD::get(playerController);
			if (hud) {
				hud->applyState();
			}
		}
	}
}

void AComplexGameState::BeginPlay() {
	Super::BeginPlay();

	UTimeTools::resetTimeDilation(GetWorld());

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return;
	}
	gameInstance->beginPlay();

	AComplexGameMode* gameMode = Cast< AComplexGameMode >(GetWorld()->GetAuthGameMode());

	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		gameStage = gameMode->gameStage;
		if (gameStage == EComplexGameStage::MAIN_MENU) {
			preGame = false;
		}
		OnRep_RefreshHUD();
	}
}

void AComplexGameState::update(float deltaTime) {

	frameTime += deltaTime;
	frameCounter++;
	if (frameTime >= 1.0f) {
		framesPerSecond = (float)frameCounter * (1.0f / frameTime);
		frameTime = 0.0f;
		frameCounter = 0;
	}
	
	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {

		if (matchBeginTravelDelay > 0.0f) {
			matchBeginTravelDelay -= deltaTime;
			if (matchBeginTravelDelay <= 0.0f) {
				APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				FString command = "ServerTravel " + gameConfiguration->mapKey;
				controller->ConsoleCommand(*command, true);
				return;
			}
		}

		if (gameConfiguration == nullptr || !gameConfiguration->initialized) {
			initGameConfiguration();
			if (gameConfiguration == nullptr) {
				return;
			}
		}

		AComplexGameMode* gameMode = Cast< AComplexGameMode >(GetWorld()->GetAuthGameMode());
		if (gameMode != nullptr) {
			gameStage = gameMode->gameStage;
		}

		updatePlayers(deltaTime);

		if (preGame && gameStage == EComplexGameStage::IN_GAME) {
			gameStartDelay -= UTimeTools::paused ? deltaTime : FApp::GetDeltaTime();
			if (gameStartDelay <= 0.0f) {
				preGame = false;
				OnRep_RefreshHUD();
			}
			updateSpawning(deltaTime);
		}
		else if (gameOver && gameStage == EComplexGameStage::IN_GAME) {
			gameEndDelay -= UTimeTools::paused ? deltaTime : FApp::GetDeltaTime();
			if (gameEndDelay <= 0.0f) {
				notifyMatchReturningToMainMenu();
				UGameTools::quitToMainMenu(this,false);
			}
		}
		else {
			updateSpawning(deltaTime);
		}
	}
	else {
		if (UUserManager::instance) {
			for (auto& profileEntry : UUserManager::instance->loadedProfiles) {
				for (UPlayerRecord* player : players) {
					if (!player->isBot && player->displayName.Equals(profileEntry.Value->username)) {
						player->userProfile = profileEntry.Value;
						player->controller = UGameplayStatics::GetPlayerController(GetWorld(), profileEntry.Key);
					}
				}
			}
		}

	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (gameInstance && gameInstance->traveling) {
		AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
		if (hud) {
			hud->prompt("MatchStarting");
		}
		if (preGame) {
			notifyMatchPreGameStarted();
		}
	}
	if (gameInstance && gameInstance->returningToMainMenu) {
		AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
		if (hud) {
			if (gameStage == EComplexGameStage::MAIN_MENU) {
				hud->removePrompt("ReturningToMainMenu");
				gameInstance->returningToMainMenu = false;
			}
			else {
				hud->prompt("ReturningToMainMenu");
			}
		}
	}

	if (UUserManager::instance && gameConfiguration) {
		UUserManager::instance->ping(deltaTime, getPlayerActivity(), getMatchDisplayName(), getLevelName(), getMatchStatus(), getMatchPlayerCount(), getMatchPlayerMax());
	}
}

void AComplexGameState::updatePlayers(float deltaTime) {

	AComplexGameMode* gameMode = Cast< AComplexGameMode >(GetWorld()->GetAuthGameMode());
	if (!gameMode) {
		return;
	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return;
	}

	/*
		Ensure all players are recorded.
	*/
	int localPlayerCount = UControllerTools::getLocalPlayerCount(GetWorld());
	int playerControllers = 0;
	TArray< AComplexHUD* > huds;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			playerControllers++;

			UPlayerRecord* player = getPlayerRecord(playerController);
			if (!player) {
				player = NewObject< UPlayerRecord >(gameInstance);
				player->displayName = "Human Player";
				player->controller = playerController;
				player->isBot = false;
				players.Add(player);
				AddReplicatedSubObject(player);
			}

			if (!player->profileIdentified) {

				if (player->userProfile == nullptr && playerController->IsLocalController() && UUserManager::instance != nullptr && UUserManager::instance->loaded) {
					player->userProfile = UUserManager::instance->getProfile(playerController);
					if (player->userProfile) {
						player->profileIdentified = true;
					}

					AComplexHUD* hud = AComplexHUD::get(playerController);
					if (hud) {
						if (player->userProfile) {
							hud->removePrompt("PleaseWait");
							player->displayName = player->userProfile->username;
						}
						else {
							if (UUserManager::instance && UUserManager::instance->autoLoggingIn) {
								hud->prompt("PleaseWait");
							}
							else {
								hud->prompt("UserLogin");
							}
						}
					}
				}

				if (!playerController->IsLocalController()) {
					AComplexPlayerController* complexController = Cast< AComplexPlayerController >(playerController);
					if (!complexController) {
						player->profileIdentified = true;
					}
					else {
						complexController->requestProfileInfo();
					}
				}

			}

			AComplexHUD* hud = AComplexHUD::get(playerController);
			if (hud && !huds.Contains(hud)) {
				huds.Add(hud);
			}

		}
	}
	UDebug::tick("Local Players: " + FString::FromInt(localPlayerCount) + ", Controllers: " + FString::FromInt(playerControllers) + ", Unique HUDs: " + FString::FromInt(huds.Num()));

	/*
		Remove dead human player records.
	*/
	for (int i = 0; i < players.Num(); i++) {
		if (!players[i]->isBot && players[i]->controller == nullptr) {

			if (players[i]->pawn) {
				players[i]->pawn->Destroy();
			}

			sendMessage(players[i]->displayName + " has left", FColor::Silver, "");

			UTeam* team = getTeam(players[i]);
			if (team) {
				team->players.Remove(players[i]);
			}
			RemoveReplicatedSubObject(players[i]);
			players.RemoveAt(i);
			i--;
		}
	}

	/*
		Assign human player teams and bots for idle players
	*/
	for (UPlayerRecord* player : players) {
		if (!player->isBot) {

			if (getTeam(player) == nullptr) {
				UTeam* bestTeam = nullptr;
				double bestTeamPerc = 0.0;

				for (UTeam* team : teams) {
					int maxCount = FMath::Max(1, team->maxPlayerCount);
					int humanCount = team->getHumanPlayerCount();
					if (humanCount < maxCount) {
						double humanPerc = (double)humanCount / (double)maxCount;
						if (bestTeam == nullptr || humanPerc < bestTeamPerc) {
							bestTeam = team;
							bestTeamPerc = humanPerc;
						}
						else if (humanPerc == bestTeamPerc && FMath::RandRange(1, 100) <= 50) {
							bestTeam = team;
							bestTeamPerc = humanPerc;
						}
					}
				}

				if (bestTeam) {
					player->team = bestTeam;
					bestTeam->players.Add(player);
				}
			}

			if (player->lastInputTimer < gameConfiguration->idlePlayerBotTime) {
				player->lastInputTimer += deltaTime;
			}


			if (gameConfiguration->autoBotIdlePlayers && player->pawn != nullptr && player->lastInputTimer >= gameConfiguration->idlePlayerBotTime && UMindComponent::get(player->pawn) == nullptr) {
				UMindComponent::add(player->pawn);
			}
		}
	}

	/*
		Spawn NPCs to fill the ranks.
	*/
	if (gameStage == EComplexGameStage::IN_GAME) {
		for (UTeam* team : teams) {
		if (team->players.Num() < team->maxPlayerCount) {
			int addCount = team->maxPlayerCount - team->players.Num();
			for (int i = 0; i < addCount; i++) {
				UPlayerRecord* player = NewObject< UPlayerRecord >(gameInstance);
				player->displayName = team->botNames.Num() > 0 ? team->getNextBotName() : getNextBotName();
				player->isBot = true;
				player->profileIdentified = true;
				players.Add(player);
				player->team = team;
				team->players.Add(player);
				AddReplicatedSubObject(player);
			}
		}
	}
}

	/*
		Remove NPC overflow.
	*/
	for (UTeam* team : teams) {
		if (team->players.Num() > team->maxPlayerCount || gameStage == EComplexGameStage::MAIN_MENU) {
			UPlayerRecord* botToRemove = nullptr;

			for (UPlayerRecord* player : team->players) {
				if (player->isBot) {
					botToRemove = player;
				}
			}

			if (botToRemove) {
				if (botToRemove->pawn) {
					botToRemove->pawn->Destroy();
				}
				team->players.Remove(botToRemove);
				RemoveReplicatedSubObject(botToRemove);
				players.Remove(botToRemove);
			}
		}
	}

	/*
		Remove orphaned NPCs
	*/
	for (int i = 0; i < players.Num(); i++) {
		UTeam* team = getTeam(players[i]);
		if (players[i]->isBot && (team == nullptr || gameStage == EComplexGameStage::MAIN_MENU)) {
			if (players[i]->pawn) {
				players[i]->pawn->Destroy();
			}
			if (team) {
				team->players.Remove(players[i]);
			}
			RemoveReplicatedSubObject(players[i]);
			players.RemoveAt(i);
			i--;
		}
	}

}

bool AComplexGameState::isValidPawn(UPlayerRecord* player, APawn* pawn) {
	if (gameStage == EComplexGameStage::MAIN_MENU && !player->isBot) {
		return true;
	}
	return player->getPawn() != nullptr && player->getPawn()->IsA(ACreature::StaticClass());
}


void AComplexGameState::updateSpawning(float deltaTime) {

	if (generalSpawningDelay > 0.0f) {
		generalSpawningDelay -= deltaTime;
		return;
	}
	if (preGame) {
		return;
	}

	AComplexGameMode* gameMode = Cast< AComplexGameMode >(GetWorld()->GetAuthGameMode());
	if (!gameMode) {
		return;
	}

	for (UPlayerRecord* player : players) {
		if (!isValidPawn(player, player->getPawn())) {

			if (player->isBot) {
				APawn* pawn = spawnPawn(player);
				if (pawn) {
					pawn->SpawnDefaultController();
					player->controller = pawn->Controller;
					player->pawn = pawn;
					UMindComponent::add(pawn);
				}
			}
			else if(player->controller){
				APawn* pawn = spawnPawn(player);
				if (pawn) {
					player->controller->Possess(pawn);
					player->pawn = pawn;
					if (gameConfiguration->spawnPlayersAsBots) {
						UMindComponent::add(player->pawn);
					}
				}
			}

		}
	}
}


void AComplexGameState::tearDown() {
	UTimeTools::resetTimeDilation(GetWorld());

	ASceneObject::destroyAll();
	ACreature::destroyAll();
	ADropItem::destroyAll();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = Cast< AComplexHUD >(playerController->GetHUD());
			if (hud) {
				hud->removeAllLayers();
			}

			AComplexPlayerController* complexController = Cast< AComplexPlayerController >(playerController);
			if (complexController) {
				complexController->paused = false;
			}
		}
	}
}

void AComplexGameState::handleLoadComplete() {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = Cast< AComplexHUD >(playerController->GetHUD());
			if (hud) {
				hud->setState(AComplexHUD::STATE_DEFAULT);
			}
		}
	}
}

void AComplexGameState::startPauseChange() {

	int pausedControllers = 0;
	int totalControllers = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		totalControllers++;

		AComplexPlayerController* playerController = Cast< AComplexPlayerController >(Iterator->Get());
		if (playerController && playerController->paused) {
			pausedControllers++;
		}
	}

	bool controllersPaused = false;
	if (pausedControllers > 0) {
		AComplexGameMode* gameMode = Cast< AComplexGameMode >(GetWorld()->GetAuthGameMode());
		if (pausedControllers == totalControllers && gameMode && gameConfiguration->pauseStopsTime) {
			controllersPaused = true;
		}
	}

	executePauseChange(controllersPaused);
}

void AComplexGameState::executePauseChange(bool controllersPaused) {
	if (controllersPaused) {
		UTimeTools::pauseTime(GetWorld());
	}
	else {
		UTimeTools::resumeTime(GetWorld());
	}
}

void AComplexGameState::startEndGame() {
	gameOver = true;

	UTimeTools::setPreferredTimeDilation(GetWorld(), 0.25f);

	UTeam* localTeam = UTeam::get(GetWorld()->GetFirstPlayerController());
	UTeam* winningTeam = nullptr;
	for (UTeam* team : teams) {
		if (team->kills >= gameConfiguration->killsToWin) {
			winningTeam = team;
			break;
		}
	}
	if (winningTeam == localTeam) {
		//UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("Victory"), 1.0f, 1.0f, 0.0f);
	}
	else {
		//UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("Defeated"), 1.0f, 1.0f, 0.0f);
	}

	OnRep_RefreshHUD();
}

UPlayerRecord* AComplexGameState::getPlayerRecord(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	for (UPlayerRecord* player : players) {
		if (!player) {
			continue;
		}
		if (player->pawn == actor) {
			return player;
		}
	}
	return nullptr;
}

UPlayerRecord* AComplexGameState::getPlayerRecord(AController* controller) {
	if (controller == nullptr) {
		return nullptr;
	}
	for (UPlayerRecord* player : players) {
		if (player->controller == controller) {
			return player;
		}
	}
	return nullptr;
}

bool AComplexGameState::isLocalPlayer(UPlayerRecord* player) {
	return player->controller && player->controller->IsLocalController() && player->controller->IsA(APlayerController::StaticClass());
}

UTeam* AComplexGameState::getTeam(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	UPlayerRecord* player = getPlayerRecord(actor);
	if (player) {
		return getTeam(player);
	}
	return nullptr;
}

UTeam* AComplexGameState::getTeam(AController* controller) {
	if (controller == nullptr) {
		return nullptr;
	}
	UPlayerRecord* player = getPlayerRecord(controller);
	if (player) {
		return getTeam(player);
	}
	return nullptr;
}

UTeam* AComplexGameState::getTeam(UPlayerRecord* player) {
	if (player == nullptr) {
		return nullptr;
	}
	if (player->team != nullptr) {
		return player->team;
	}
	for (UTeam* team : teams) {
		if (team->players.Contains(player)) {
			return team;
		}
	}
	return nullptr;
}


int AComplexGameState::getHumanPlayerCount() {
	int total = 0;
	for (UPlayerRecord* player : players) {
		if (!player->isBot) {
			total++;
		}
	}
	return total;
}

int AComplexGameState::getBotPlayerCount() {
	int total = 0;
	for (UPlayerRecord* player : players) {
		if (player->isBot) {
			total++;
		}
	}
	return total;
}

APawn* AComplexGameState::spawnPawn(UPlayerRecord* player) {
	UTeam* team = getTeam(player);
	if (team == nullptr) {
		return nullptr;
	}

	TArray< int > spawnOptionIndices;
	for (int i = 0; i < team->spawnOptions.Num(); i++) {
		if (team->spawnOptions[i].actorClass == nullptr) {
			continue;
		}
		if (team->spawnOptions[i].maxCountPerTeam > 0 && team->spawnOptions[i].maxCountPerTeam <= team->getPawnsOfClassCount(team->spawnOptions[i].actorClass)) {
			continue;
		}
		spawnOptionIndices.Add(i);
	}

	if (spawnOptionIndices.Num() == 0) {
		return nullptr;
	}

	FSpawnOption& spawnOption = team->spawnOptions[spawnOptionIndices[FMath::RandRange(0, spawnOptionIndices.Num() - 1)]];
	FTransform spawnTransform = getSpawnPoint(player);

	FActorSpawnParameters params;
	params.bNoFail = true;
	APawn* pawn = GetWorld()->SpawnActor<APawn>((UClass*)spawnOption.actorClass, spawnTransform.GetLocation(), FRotator::ZeroRotator, params);
	if (pawn && pawn->IsA(ACreature::StaticClass())) {
		ACreature* creature = Cast< ACreature >(pawn);
		creature->team = team;
		creature->bodyYaw = spawnTransform.GetRotation().Rotator().Yaw;
		creature->controlRotation = spawnTransform.GetRotation().Rotator();
	}
	return pawn;
}

FTransform AComplexGameState::getSpawnPoint(UPlayerRecord* player) {
	FTransform transform;

	TArray< ASpawnPoint* > spawnPoints = ASpawnPoint::getAll();
	if (spawnPoints.Num() > 0) {
		ASpawnPoint* spawnPoint = spawnPoints[FMath::RandRange(0, spawnPoints.Num()-1)];
		transform.SetLocation(FVector(0, 0, 100) + spawnPoint->GetActorLocation());
		transform.SetRotation(spawnPoint->GetActorRotation().Quaternion());
		return transform;
	}

	transform.SetLocation(FVector(0, 0, 100) + UMathTools::randomVectorFromRange(gameConfiguration->defaultSpawnAreaExtent.X, gameConfiguration->defaultSpawnAreaExtent.Y, gameConfiguration->defaultSpawnAreaExtent.Z));
	transform.SetRotation(FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f).Quaternion());
	return transform;
}

void AComplexGameState::despawnPawn(APawn* pawn) {
	UPlayerRecord* player = getPlayerRecord(pawn);
	if (player) {
		player->pawn = nullptr;
		if (player->isBot) {
			player->controller = nullptr;
		}
	}
}

void AComplexGameState::scoreDamage(FDamageDetails damage, int totalDamage) {

	if (totalDamage <= 0) {
		return;
	}

	if (preGame || gameOver) {
		return;
	}

	if (!gameConfiguration || gameConfiguration->scoreMultiplierDamage <= 0.001f) {
		return;
	}


	ACreature* attackingCreature = damage.attacker.getCreature(GetWorld());
	UPlayerRecord* player = getPlayerRecord(attackingCreature);
	if (player) {
		int score = FMath::Max(1, totalDamage * gameConfiguration->scoreMultiplierDamage);
		UTeam* team = getTeam(player);
		if (gameConfiguration->negativeScoreForFriendlyFire && !UTeam::isHostile(team, UTeam::get(GetWorld(), damage.victim))) {
			score *= -1;
		}

		player->score += score;
		if (team) {
			team->score += score;
		}
	}
}

void AComplexGameState::scoreKill(FDamageDetails damage, int totalDamage) {

	if (preGame || gameOver) {
		return;
	}

	if (!gameConfiguration || gameConfiguration->scoreMultiplierKill <= 0.001f) {
		return;
	}

	ACreature* attackingCreature = damage.attacker.getCreature(GetWorld());
	if (attackingCreature) {
		UPlayerRecord* player = getPlayerRecord(attackingCreature);
		if (player) {

			int score = FMath::Max(1, gameConfiguration->scoreMultiplierKill);
			UTeam* team = getTeam(player);
			bool allyKill = !UTeam::isHostile(team, UTeam::get(GetWorld(), damage.victim));
			if (gameConfiguration->negativeScoreForFriendlyFire && allyKill) {
				score *= -1;
			}

			player->score += score;
			if (team) {
				team->score += score;
			}

			if (!allyKill) {
				player->kills++;
				team->kills++;

				if (gameConfiguration->killsToWin > 0 && team->kills >= gameConfiguration->killsToWin) {
					startEndGame();
				}
			}

		}
	}

	ACreature* victimCreature = damage.victim.getCreature(GetWorld());
	if (victimCreature) {
		UPlayerRecord* player = getPlayerRecord(victimCreature);
		if (player) {
			player->deaths++;
			UTeam* team = getTeam(player);
			if (team) {
				team->deaths++;
			}
		}
	}

	sendScoreKill(damage, totalDamage);
}

void AComplexGameState::scoreDeath(APawn* pawn, FString killerName, FColor killerColor, FString killerIcon) {
	ACreature* victimCreature = Cast<ACreature>(pawn);
	if (victimCreature) {
		UPlayerRecord* player = getPlayerRecord(victimCreature);
		if (player) {
			player->deaths++;
			UTeam* team = getTeam(player);
			if (team) {
				team->deaths++;
			}
			sendTargetMessage(FTargetDetail(victimCreature), killerName, killerColor, killerIcon);
		}
	}
}

void AComplexGameState::sendScoreKill_Implementation(FDamageDetails damage, int totalDamage) {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = AComplexHUD::get(playerController);
			if (hud) {
				UPlayerHUD* playerHUD = Cast< UPlayerHUD >(hud->getWidget(UPlayerHUD::StaticClass()));
				if (playerHUD) {
					playerHUD->alertKill(damage);
				}
			}
		}
	}
}

void AComplexGameState::sendTargetMessage_Implementation(FTargetDetail target, const FString& messageName, FColor messageColor, const FString& messageIconKey){
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = AComplexHUD::get(playerController);
			if (hud) {
				UPlayerHUD* playerHUD = Cast< UPlayerHUD >(hud->getWidget(UPlayerHUD::StaticClass()));
				if (playerHUD) {
					playerHUD->alertToTarget(target, messageName, messageColor, messageIconKey);
				}
			}
		}
	}
}

void AComplexGameState::sendMessage_Implementation(const FString& messageName, FColor messageColor, const FString& messageIconKey) {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* playerController = Cast< APlayerController >(Iterator->Get());
		if (playerController) {
			AComplexHUD* hud = AComplexHUD::get(playerController);
			if (hud) {
				UPlayerHUD* playerHUD = Cast< UPlayerHUD >(hud->getWidget(UPlayerHUD::StaticClass()));
				if (playerHUD) {
					playerHUD->alert(messageName, messageColor, messageIconKey);
				}
			}
		}
	}
}

int AComplexGameState::getFramesPerSecond() {
	return framesPerSecond;
}


bool AComplexGameState::nameIsUsed(FString name) {
	for (UPlayerRecord* player : players) {
		if (player->displayName.Equals(name)) {
			return true;
		}
	}
	return false;
}
FString AComplexGameState::getNextBotName() {
	if (gameConfiguration->botNames.Num() > 0) {
		bool keepTrying = true;
		int level = 1;
		while (keepTrying) {

			TArray< FString > possibleNames;
			for (FString botName : gameConfiguration->botNames) {
				FString actualName = level == 1 ? botName : botName + " " + FString::FromInt(level);
				if (!nameIsUsed(actualName)) {
					possibleNames.Add(actualName);
				}
			}

			if (possibleNames.Num() > 0) {
				return possibleNames[FMath::RandRange(0, possibleNames.Num() - 1)];
			}

			level++;
		}

	}
	return "AI Bot";
}

FString AComplexGameState::getPlayerActivity() {
	if (gameStage == EComplexGameStage::MAIN_MENU) {
		if (inLobby) {
			return "In-Lobby";
		}
		return "Main Menu";
	}
	else {
		return "In-Game";
	}
}

FString AComplexGameState::getMatchDisplayName() {
	return gameConfiguration->displayName;
}

FString AComplexGameState::getMatchStatus() {
	if (gameStage == EComplexGameStage::MAIN_MENU) {
		if (inLobby) {
			return "In-Lobby";
		}
		return "NA";
	}
	else {
		if (preGame) {
			return "Pre-Game";
		}
		else if (gameOver) {
			return "End-Game";
		}
		return "In-Game";
	}
}

FString AComplexGameState::getLevelName() {
	return GetWorld()->GetMapName();
}

int AComplexGameState::getMatchPlayerCount() {
	int playerCount = 0;
	for (UPlayerRecord* player : players) {
		if (!player->isBot) {
			playerCount++;
		}
	}
	return playerCount;
}

int AComplexGameState::getMatchPlayerMax() {
	int playerCount = 0;
	for (UTeam* team : teams) {
		playerCount += team->maxPlayerCount;
	}
	return playerCount;
}

void AComplexGameState::openLobby() {
	inLobby = true;
	if (UUserManager::instance) {
		UUserManager::instance->pingTime = 0.0f;
	}
}

void AComplexGameState::leaveLobby() {
	inLobby = false;
	if (UUserManager::instance) {
		UUserManager::instance->pingTime = 0.0f;
	}
}

void AComplexGameState::handlePlayerLeave(APlayerController* playerController) {
	for (int i = 0; i < players.Num(); i++) {
		if (players[i]->controller == playerController) {
			players[i]->controller = nullptr;
			APawn* pawn = playerController->GetPawn();
			if (pawn) {
				pawn->Destroy();
			}
		}
	}
}

void AComplexGameState::initGameConfiguration() {

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return;
	}

	gameConfiguration = gameInstance->gameConfiguration;
	if (gameConfiguration == nullptr) {
		UGameConfiguration* baseGameConfiguration = UResourceCache::getGameConfiguration("TestPlay");
		if (baseGameConfiguration) {
			gameInstance->gameConfiguration = NewObject< UGameConfigurationInstance >(gameInstance);
			gameInstance->gameConfiguration->copyFromBase(baseGameConfiguration);
			gameInstance->gameConfiguration->baseConfiguration = baseGameConfiguration;
			gameInstance->teams.Empty();
			gameInstance->players.Empty();
			gameConfiguration = gameInstance->gameConfiguration;
		}
	}
	if (gameConfiguration == nullptr) {
		gameConfiguration = NewObject< UGameConfigurationInstance >(gameInstance);
		gameInstance->gameConfiguration = gameConfiguration;
	}
	teams = gameInstance->teams;
	players = gameInstance->players;
	inLobby = gameInstance->inLobby;

	if (!gameConfiguration->initialized) {
		teams.Empty();
		if (gameConfiguration->baseConfiguration) {
			syncFromBaseConfiguration(gameConfiguration->baseConfiguration);
		}
	}
	else {

		for (UTeam* team : teams) {
			team->resetScore();
			team->players.Empty();
		}
		for (UPlayerRecord* player : players) {
			player->resetScore();
		}
	}

	AddReplicatedSubObject(gameConfiguration);
	for (UTeam* team : teams) {
		AddReplicatedSubObject(team);
	}
	for (UPlayerRecord* player : players) {
		AddReplicatedSubObject(player);
	}

	gameStartDelay = gameConfiguration->gameStartDelay;
	gameEndDelay = gameConfiguration->gameEndDelay;
	generalSpawningDelay = gameConfiguration->generalSpawningDelay;
	gameConfiguration->initialized = true;

	OnRep_RefreshHUD();
}

void AComplexGameState::syncFromBaseConfiguration(UGameConfiguration* base) {

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return;
	}

	for (FTeamSpec& teamSpec : base->teams) {
		UTeam* team = NewObject< UTeam >(gameInstance);

		team->displayName = teamSpec.displayName;
		team->color = teamSpec.color;
		team->spawnOptions = teamSpec.spawnOptions;
		team->maxPlayerCount = teamSpec.maxPlayerCount;
		team->friendlyFireDamage = teamSpec.friendlyFireDamage;
		team->defaultMindComponentClass = teamSpec.defaultMindComponentClass;
		team->botNames = teamSpec.botNames;

		teams.Add(team);
	}
}

void AComplexGameState::syncToGameInstance() {
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return;
	}
	gameInstance->gameConfiguration = gameConfiguration;
	gameInstance->players = players;
	gameInstance->teams = teams;
	gameInstance->inLobby = inLobby;
}

void AComplexGameState::searchMatches(FString preferredDisplayName, FString preferredLevelName, bool includeLobby, bool includeInGame, bool includeFull, bool autoJoin) {

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	UPlayerRecord* player = getPlayerRecord(controller);
	if (player == nullptr) {
		return;
	}
	UUserProfile* profile = player->userProfile;
	if (profile == nullptr || profile->local) {
		return;
	}

	UMatchSearchTransaction* transaction = Cast< UMatchSearchTransaction > ( UHTTPManager::instance->initTransaction(UMatchSearchTransaction::StaticClass()) );
	transaction->preferredDisplayName = preferredDisplayName;
	transaction->preferredLevelName = preferredLevelName;
	transaction->includeLobby = includeLobby;
	transaction->includeInGame = includeInGame;
	transaction->includeFull = includeFull;
	transaction->autoJoin = autoJoin;

	transaction->onSuccess.AddDynamic(this, &AComplexGameState::onSearchedMatches);
	transaction->onFailure.AddDynamic(this, &AComplexGameState::onSearchedMatches);

	TMap<FString, FString > headers;
	TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
	UJsonTools::setString(contents, "username", profile->username);
	UJsonTools::setString(contents, "token", profile->token);
	UJsonTools::setString(contents, "gameName", UGameTools::getGameNameFromWorld(GetWorld()));

	UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/searchSessions.php", headers, UJsonTools::toString(contents, false));

	AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
	if (hud) {
		hud->prompt("SearchingForMatch");
	}
}

void AComplexGameState::startSearchMatches(AActor* actor, FString preferredDisplayName, FString preferredLevelName, bool includeLobby, bool includeInGame, bool includeFull, bool autoJoin) {
	UWorld* world = actor->GetWorld();
	AComplexGameState* gameState = Cast<AComplexGameState>(world->GetGameState());
	if (gameState) {
		gameState->searchMatches(preferredDisplayName, preferredLevelName, includeLobby, includeInGame, includeFull, autoJoin);
	}
}

void AComplexGameState::onSearchedMatches(UHTTPTransaction* transaction, FString response) {
	
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	UPlayerRecord* player = getPlayerRecord(controller);
	if (player == nullptr) {
		return;
	}
	UUserProfile* profile = player->userProfile;
	if (profile == nullptr || profile->local) {
		return;
	}
	
	UMatchSearchTransaction* searchTransaction = Cast< UMatchSearchTransaction >(transaction);
	TArray< FMatchDetail > matchDetails;

	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(response);
	if (root != nullptr) {
		//UDebug::print("Matches: " + UJsonTools::toString(root, false));

		FString results = UJsonTools::getString(root, "results");
		TArray < TSharedPtr<FJsonValue> > resultsValues = UJsonTools::parseJSONArray(results);

		for (TSharedPtr<FJsonValue> resultValue : resultsValues) {
			TSharedPtr<FJsonObject> result = resultValue->AsObject();
			//UDebug::print("Match: " + UJsonTools::toString(result, false));

			FMatchDetail matchDetail;

			matchDetail.hostName = UJsonTools::getString(result, "Name");
			matchDetail.level = UJsonTools::getString(result, "LevelName").Replace(TEXT("UEDPIE_0_"), TEXT(""));
			matchDetail.displayName = UJsonTools::getString(result, "DisplayName");
			matchDetail.maxPlayers = UJsonTools::getNumeric(result, "MaxPlayers");
			matchDetail.currentPlayers = UJsonTools::getNumeric(result, "CurrentPlayers");
			matchDetail.status = UJsonTools::getString(result, "Status");
			matchDetail.ipAddress = UJsonTools::getString(result, "IPAddress");

			if (matchDetail.hostName.Equals(profile->username)) {
				//UDebug::print("Match has same user as local player: " + profile->username);
				continue;
			}
			if (!searchTransaction->includeLobby && matchDetail.status.Equals("In-Lobby")) {
				//UDebug::print("Match is in lobby. We don't want that.");
				continue;
			}
			if (!searchTransaction->includeInGame && matchDetail.status.Equals("In-Game")) {
				//UDebug::print("Match is in game. We don't want that.");
				continue;
			}
			if (!searchTransaction->includeFull && matchDetail.maxPlayers <= matchDetail.currentPlayers) {
				//UDebug::print("Match is full! " + FString::FromInt(matchDetail.currentPlayers) + " vs " + FString::FromInt(matchDetail.maxPlayers));
				continue;
			}
			if (searchTransaction->preferredDisplayName.Len() > 0 && !searchTransaction->preferredDisplayName.Equals(matchDetail.displayName)) {
				//UDebug::print("Match doesn't have preferred name/mode '" + searchTransaction->preferredDisplayName + "' vs '" + matchDetail.displayName + "'");
				continue;
			}
			if (searchTransaction->preferredLevelName.Len() > 0 && !searchTransaction->preferredLevelName.Equals(matchDetail.level)) {
				//UDebug::print("Match doesn't have preferred level '" + searchTransaction->preferredLevelName + "' vs '" + matchDetail.level + "'");
				continue;
			}

			matchDetails.Add(matchDetail);
		}
	}

	AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
	if (hud) {
		hud->removePrompt("SearchingForMatch");
	}

	finishSearchMatches(searchTransaction, matchDetails);
}

void AComplexGameState::finishSearchMatches(UMatchSearchTransaction* transaction, TArray< FMatchDetail > results) {

	if (results.Num() == 0) {
		AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
		if (hud) {
			hud->prompt("NoMatchesFound");
		}
		return;
	}
	
	if (transaction->autoJoin) {
		FString ipAddress = results[0].ipAddress;
		if (UHTTPTools::getExternalIP().Equals(ipAddress)) {
			ipAddress = "127.0.0.1";
		}
		UGameTools::joinIP(GetWorld(), ipAddress);
	}

}

void AComplexGameState::notifyMatchStart_Implementation() {
	AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
	if (hud) {
		hud->prompt("MatchStarting");
	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (gameInstance) {
		gameInstance->traveling = true;
	}
}

void AComplexGameState::notifyMatchJoining() {
	AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
	if (hud) {
		hud->prompt("MatchJoining");
	}
}

void AComplexGameState::notifyMatchPreGameStarted_Implementation() {
	AComplexHUD* hud = AComplexHUD::get(GetWorld()->GetFirstPlayerController());
	if (hud) {
		hud->removePrompt("MatchStarting");
	}

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (gameInstance) {
		gameInstance->traveling = false;
	}
}

void AComplexGameState::notifyMatchReturningToMainMenu_Implementation() {
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (gameInstance) {
		gameInstance->returningToMainMenu = true;
	}
}


EComplexGameStage AComplexGameState::getGameStage(UWorld* world) {
	if (world) {
		AComplexGameState* gameState = Cast< AComplexGameState >(world->GetGameState());
		if (gameState) {
			return gameState->gameStage;
		}
	}
	return EComplexGameStage::MAIN_MENU;
}