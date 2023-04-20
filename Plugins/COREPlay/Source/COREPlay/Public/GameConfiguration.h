#pragma once

#include "CoreMinimal.h"
#include "TeamSpec.h"
#include "Team.h"
#include "PlayerRecord.h"

#include "Net/UnrealNetwork.h"


#include "GameConfiguration.generated.h"


UCLASS()
class COREPLAY_API UGameConfiguration : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FTeamSpec > teams;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > mapOptions;

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		FString description;

	UPROPERTY(EditAnywhere)
		float scoreMultiplierDamage = 1.0f;

	UPROPERTY(EditAnywhere)
		float scoreMultiplierKill = 100.0f;

	UPROPERTY(EditAnywhere)
		bool negativeScoreForFriendlyFire = true;

	UPROPERTY(EditAnywhere)
		bool spawnPlayersAsBots = false;

	UPROPERTY(EditAnywhere)
		bool autoBotIdlePlayers = true;

	UPROPERTY(EditAnywhere)
		float idlePlayerBotTime = 60.0f;

	UPROPERTY(EditAnywhere)
		float generalSpawningDelay = 5.0f;

	UPROPERTY(EditAnywhere)
		float gameStartDelay = 10.0f;

	UPROPERTY(EditAnywhere)
		float gameEndDelay = 30.0f;

	UPROPERTY(EditAnywhere)
		int killsToWin = -1;

	UPROPERTY(EditAnywhere)
		bool pauseStopsTime = true;

	UPROPERTY(EditAnywhere)
		FVector defaultSpawnAreaExtent = FVector(100, 100, 0);

	UPROPERTY(EditAnywhere)
		bool debugPathing = false;

	UPROPERTY(EditAnyWhere)
		TArray< FString > botNames;

};

UCLASS()
class COREPLAY_API UGameConfigurationInstance : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		UGameConfiguration* baseConfiguration = nullptr;

	UPROPERTY(replicated)
		FString mapKey;

	UPROPERTY(replicated)
		FString displayName;

	UPROPERTY(replicated)
		FString description;

	UPROPERTY(replicated)
		float scoreMultiplierDamage = 1.0f;

	UPROPERTY(replicated)
		float scoreMultiplierKill = 100.0f;

	UPROPERTY(replicated)
		bool negativeScoreForFriendlyFire = true;

	UPROPERTY(replicated)
		bool spawnPlayersAsBots = false;

	UPROPERTY(replicated)
		bool autoBotIdlePlayers = true;

	UPROPERTY(replicated)
		float idlePlayerBotTime = 60.0f;

	UPROPERTY(replicated)
		float generalSpawningDelay = 5.0f;

	UPROPERTY(replicated)
		float gameStartDelay = 10.0f;

	UPROPERTY(replicated)
		float gameEndDelay = 30.0f;

	UPROPERTY(replicated)
		int killsToWin = -1;

	UPROPERTY(replicated)
		bool pauseStopsTime = true;

	UPROPERTY(replicated)
		FVector defaultSpawnAreaExtent = FVector(100, 100, 0);

	UPROPERTY(replicated)
		bool debugPathing = false;

	UPROPERTY(replicated)
		TArray< FString > botNames;

	virtual bool IsSupportedForNetworking() const override {
		return true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass())) {
			BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
		}
		DOREPLIFETIME(UGameConfigurationInstance, mapKey);
		DOREPLIFETIME(UGameConfigurationInstance, displayName);
		DOREPLIFETIME(UGameConfigurationInstance, description);
		DOREPLIFETIME(UGameConfigurationInstance, scoreMultiplierDamage);
		DOREPLIFETIME(UGameConfigurationInstance, scoreMultiplierKill);
		DOREPLIFETIME(UGameConfigurationInstance, negativeScoreForFriendlyFire);
		DOREPLIFETIME(UGameConfigurationInstance, spawnPlayersAsBots);
		DOREPLIFETIME(UGameConfigurationInstance, autoBotIdlePlayers);
		DOREPLIFETIME(UGameConfigurationInstance, idlePlayerBotTime);
		DOREPLIFETIME(UGameConfigurationInstance, generalSpawningDelay);
		DOREPLIFETIME(UGameConfigurationInstance, gameStartDelay);
		DOREPLIFETIME(UGameConfigurationInstance, gameEndDelay);
		DOREPLIFETIME(UGameConfigurationInstance, killsToWin);
		DOREPLIFETIME(UGameConfigurationInstance, pauseStopsTime);
		DOREPLIFETIME(UGameConfigurationInstance, defaultSpawnAreaExtent);
		DOREPLIFETIME(UGameConfigurationInstance, debugPathing);
		DOREPLIFETIME(UGameConfigurationInstance, botNames);
	}

	virtual void copyFromBase(UGameConfiguration* base);
};