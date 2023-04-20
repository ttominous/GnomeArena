#pragma once

#include "CoreMinimal.h"
#include "HTTPTools.h"
#include "MatchDetail.generated.h"


UCLASS()
class COREPLAY_API UMatchSearchTransaction : public UHTTPTransaction {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString preferredDisplayName;

	UPROPERTY()
		FString preferredLevelName;

	UPROPERTY()
		bool includeLobby;

	UPROPERTY()
		bool includeInGame;

	UPROPERTY()
		bool includeFull;

	UPROPERTY()
		bool autoJoin;

};

USTRUCT()
struct COREPLAY_API FMatchDetail {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString hostName;

	UPROPERTY()
		FString level;

	UPROPERTY()
		FString displayName;

	UPROPERTY()
		int maxPlayers;

	UPROPERTY()
		int currentPlayers;

	UPROPERTY()
		FString status;

	UPROPERTY()
		FString ipAddress;

};
