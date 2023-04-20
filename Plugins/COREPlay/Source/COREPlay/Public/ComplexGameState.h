#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "ComplexGameStage.h"
#include "PlayerRecord.h"
#include "DamageDetails.h"
#include "GameConfiguration.h"
#include "MatchDetail.h"

#include "Team.h"

#include "ComplexGameState.generated.h"


UCLASS()
class COREPLAY_API AComplexGameState : public AGameStateBase {

	GENERATED_BODY()

public:

	UPROPERTY(replicated, ReplicatedUsing = OnRep_RefreshHUD)
		EComplexGameStage gameStage;

	UPROPERTY()
		int framesPerSecond = 0;

	UPROPERTY()
		float frameTime = 0.0f;

	UPROPERTY()
		int frameCounter = 0;

	UPROPERTY(replicated, ReplicatedUsing = OnRep_RefreshHUD)
		bool inLobby = false;

	UPROPERTY(replicated, ReplicatedUsing = OnRep_RefreshHUD)
		bool preGame = true;

	UPROPERTY(replicated, ReplicatedUsing = OnRep_RefreshHUD)
		bool gameOver = false;

	UPROPERTY(replicated)
		float gameStartDelay;

	UPROPERTY(replicated)
		float gameEndDelay;

	UPROPERTY()
		float generalSpawningDelay;

	UPROPERTY()
		float matchBeginTravelDelay = -1.0f;

	UPROPERTY(replicated)
		UGameConfigurationInstance* gameConfiguration = nullptr;

	UPROPERTY(replicated)
		TArray< UTeam* > teams;

	UPROPERTY(replicated)
		TArray< UPlayerRecord* > players;

	AComplexGameState();
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AComplexGameState, gameStage);
		DOREPLIFETIME(AComplexGameState, gameStartDelay);
		DOREPLIFETIME(AComplexGameState, gameEndDelay);
		DOREPLIFETIME(AComplexGameState, preGame);
		DOREPLIFETIME(AComplexGameState, gameOver);
		DOREPLIFETIME(AComplexGameState, inLobby);
		DOREPLIFETIME(AComplexGameState, gameConfiguration);
		DOREPLIFETIME(AComplexGameState, players);
		DOREPLIFETIME(AComplexGameState, teams);
	}

	UFUNCTION()
		void OnRep_RefreshHUD();

	virtual void update(float deltaTime);
	virtual void updatePlayers(float deltaTime);
	virtual void updateSpawning(float deltaTime);
	virtual void tearDown();
	virtual void handleLoadComplete();
	virtual void startPauseChange();
	virtual void executePauseChange(bool controllersPaused);
	virtual void startEndGame();

	virtual UPlayerRecord* getPlayerRecord(AActor* actor);
	virtual UPlayerRecord* getPlayerRecord(AController* controller);
	virtual bool isLocalPlayer(UPlayerRecord* player);
	virtual UTeam* getTeam(AActor* actor);
	virtual UTeam* getTeam(AController* controller);
	virtual UTeam* getTeam(UPlayerRecord* player);

	virtual int getHumanPlayerCount();
	virtual int getBotPlayerCount();

	virtual bool isValidPawn(UPlayerRecord* player, APawn* pawn);
	virtual APawn* spawnPawn(UPlayerRecord* player);
	virtual void despawnPawn(APawn* pawn);
	virtual FTransform getSpawnPoint(UPlayerRecord* player);

	virtual void scoreDamage(FDamageDetails damage, int totalDamage);
	virtual void scoreKill(FDamageDetails damage, int totalDamage);
	virtual void scoreDeath(APawn* pawn, FString killerName, FColor killerColor, FString killerIcon);

	UFUNCTION(NetMulticast, Reliable)
		void sendScoreKill(FDamageDetails damage, int totalDamage);

	UFUNCTION(NetMulticast, Reliable)
		void sendTargetMessage(FTargetDetail target, const FString& messageName, FColor messageColor, const FString& messageIconKey);

	UFUNCTION(NetMulticast, Reliable)
		void sendMessage(const FString& messageName, FColor messageColor, const FString& messageIconKey);


	virtual int getFramesPerSecond();

	virtual bool nameIsUsed(FString name);
	virtual FString getNextBotName();

	virtual FString getPlayerActivity();
	virtual FString getMatchDisplayName();
	virtual FString getLevelName();
	virtual FString getMatchStatus();
	virtual int getMatchPlayerCount();
	virtual int getMatchPlayerMax();

	virtual void openLobby();
	virtual void leaveLobby();

	virtual void handlePlayerLeave(APlayerController* playerController);

	virtual void initGameConfiguration();
	virtual void syncFromBaseConfiguration(UGameConfiguration* base);
	virtual void syncToGameInstance();

	virtual void searchMatches(FString preferredDisplayName = "", FString preferredLevelName = "", bool includeLobby = true, bool includeInGame = true, bool includeFull = false, bool autoJoin = true);

	UFUNCTION(BlueprintCallable)
		static void startSearchMatches(AActor* actor, FString preferredDisplayName = "", FString preferredLevelName = "", bool includeLobby = true, bool includeInGame = true, bool includeFull = false, bool autoJoin = true);

	UFUNCTION()
		virtual void onSearchedMatches(UHTTPTransaction* transaction, FString response);

	virtual void finishSearchMatches(UMatchSearchTransaction* transaction, TArray< FMatchDetail > results);

	UFUNCTION(NetMulticast, Reliable)
		void notifyMatchStart();
	void notifyMatchJoining();

	UFUNCTION(NetMulticast, Reliable)
		void notifyMatchPreGameStarted();

	UFUNCTION(NetMulticast, Reliable)
		void notifyMatchReturningToMainMenu();

	static EComplexGameStage getGameStage(UWorld* world);

};
