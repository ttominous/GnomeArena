#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerRecord.h"
#include "SpawnOption.h"
#include "TargetDetail.h"

#include "Team.generated.h"

class UMindComponent;

UCLASS()
class COREPLAY_API UTeam : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY(replicated)
		FString displayName;

	UPROPERTY(replicated)
		FColor color;

	UPROPERTY(replicated)
		TArray< UPlayerRecord* > players;

	UPROPERTY(replicated)
		TArray< FSpawnOption > spawnOptions;

	UPROPERTY(replicated)
		int maxPlayerCount;

	UPROPERTY(replicated)
		int score = 0;

	UPROPERTY(replicated)
		int kills = 0;

	UPROPERTY(replicated)
		int deaths = 0;

	UPROPERTY(replicated)
		int assists = 0;

	UPROPERTY()
		float friendlyFireDamage;

	UPROPERTY()
		TSubclassOf< UMindComponent > defaultMindComponentClass = nullptr;

	UPROPERTY(EditAnyWhere)
		TArray< FString > botNames;

	virtual int getPawnsOfClassCount(UClass* pawnClass);
	virtual int getHumanPlayerCount();
	virtual int getBotPlayerCount();

	static UTeam* get(AActor* actor);
	static UTeam* get(AController* controller);
	static UTeam* get(UPlayerRecord* player);
	static UTeam* get(UWorld* world, FTargetDetail& targetDetail);

	virtual bool IsSupportedForNetworking() const override {
		return true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass())) {
			BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
		}
		DOREPLIFETIME(UTeam, displayName);
		DOREPLIFETIME(UTeam, color);
		DOREPLIFETIME(UTeam, players);
		DOREPLIFETIME(UTeam, spawnOptions);
		DOREPLIFETIME(UTeam, maxPlayerCount);
		DOREPLIFETIME(UTeam, score);
		DOREPLIFETIME(UTeam, kills);
		DOREPLIFETIME(UTeam, deaths);
		DOREPLIFETIME(UTeam, assists);
	}

	virtual bool isHostile(UTeam* otherTeam);
	static bool isHostile(UTeam* teamA, UTeam* teamB);

	virtual bool nameIsUsed(FString name);
	virtual FString getNextBotName();

	virtual void resetScore();

};