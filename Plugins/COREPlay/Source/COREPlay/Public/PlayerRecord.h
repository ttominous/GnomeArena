#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include "PlayerRecord.generated.h"

class UTeam;
class UUserProfile;

UCLASS()
class COREPLAY_API UPlayerRecord : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY(replicated)
		FString displayName;

	UPROPERTY(replicated)
		AController* controller = nullptr;

	UPROPERTY()
		UUserProfile* userProfile = nullptr;

	UPROPERTY(replicated)
		APawn* pawn = nullptr;

	UPROPERTY(replicated)
		bool isBot = false;

	UPROPERTY(replicated)
		UTeam* team = nullptr;

	UPROPERTY(replicated)
		int score = 0;

	UPROPERTY(replicated)
		int kills = 0;

	UPROPERTY(replicated)
		int deaths = 0;

	UPROPERTY(replicated)
		int assists = 0;

	UPROPERTY()
		float lastInputTimer = 0.0f;

	UPROPERTY()
		bool profileIdentified = false;


	virtual APawn* getPawn(bool excludeSpector = true);
	static UPlayerRecord* get(AActor* actor);
	static UPlayerRecord* get(AController* controller);

	virtual bool IsSupportedForNetworking() const override {
		return true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass())) {
			BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
		}
		DOREPLIFETIME(UPlayerRecord, displayName);
		DOREPLIFETIME(UPlayerRecord, controller);
		DOREPLIFETIME(UPlayerRecord, pawn);
		DOREPLIFETIME(UPlayerRecord, isBot);
		DOREPLIFETIME(UPlayerRecord, team);
		DOREPLIFETIME(UPlayerRecord, score);
		DOREPLIFETIME(UPlayerRecord, kills);
		DOREPLIFETIME(UPlayerRecord, deaths);
		DOREPLIFETIME(UPlayerRecord, assists);
	}

	virtual void resetScore();
};