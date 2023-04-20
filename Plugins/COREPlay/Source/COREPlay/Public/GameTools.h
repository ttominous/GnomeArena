#pragma once

#include "CoreMinimal.h"
#include "MatchDetail.h"
#include "HTTPTools.h"

#include "GameTools.generated.h"





UCLASS()
class COREPLAY_API UGameTools : public UObject {

	GENERATED_BODY()

public:

	static bool exitingApplication;
	
	static void reset();

	UFUNCTION(BlueprintCallable)
		static void markExitingApplication();

	UFUNCTION(BlueprintCallable)
		static void quitToMainMenu(AActor* actor, bool leaveLobby);

	UFUNCTION(BlueprintCallable)
		static void moveToMap(AActor* actor, FString mapKey, bool leaveLobby);

	UFUNCTION(BlueprintCallable)
		static void openLobby(AActor* actor);

	UFUNCTION(BlueprintCallable)
		static void joinIP(UWorld* world, FString ipAddress);

	UFUNCTION(BlueprintCallable)
		static FString getGameNameFromWorld(UWorld* world);

	UFUNCTION(BlueprintCallable)
		static FString getGameNameFromActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
		static bool inLobby(AActor* actor);
};
