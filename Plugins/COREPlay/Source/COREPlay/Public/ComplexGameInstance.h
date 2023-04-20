#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ResourceCache.h"
#include "SaveManager.h"
#include "HTTPManager.h"
#include "UserManager.h"
#include "MusicManager.h"
#include "GameConfiguration.h"

#include "ComplexGameInstance.generated.h"


UCLASS()
class COREPLAY_API UComplexGameInstance : public UGameInstance {

	GENERATED_BODY()

public:

	FTSTicker::FDelegateHandle tickDelegate;

	static bool isShuttingDown;

	UPROPERTY(EditAnywhere)
		FString gameName;

	UPROPERTY(EditAnywhere)
		UClass* resourceCacheClass = nullptr;

	UPROPERTY()
		UResourceCache* resourceCache = nullptr;

	UPROPERTY(EditAnywhere)
		UClass* saveManagerClass = nullptr;

	UPROPERTY()
		USaveManager* saveManager = nullptr;

	UPROPERTY(EditAnywhere)
		UClass* httpManagerClass = nullptr;

	UPROPERTY()
		UHTTPManager* httpManager = nullptr;

	UPROPERTY(EditAnywhere)
		UClass* userManagerClass = nullptr;

	UPROPERTY()
		UUserManager* userManager = nullptr;

	UPROPERTY(EditAnywhere)
		UClass* musicManagerClass = nullptr;

	UPROPERTY()
		UMusicManager* musicManager = nullptr;

	UPROPERTY(EditAnywhere)
		TArray< UClass* > gameModeClasses;

	UPROPERTY()
		bool routeFirstGamePadToPlayerOne = false;

	UPROPERTY()
		bool routeAllGamePadToPlayerOne = false;

	UPROPERTY()
		bool firstBeginPlay = true;

	UPROPERTY()
		UGameConfigurationInstance* gameConfiguration = nullptr;

	UPROPERTY()
		bool inLobby = false;

	UPROPERTY()
		bool traveling = false;

	UPROPERTY()
		bool returningToMainMenu = false;

	UPROPERTY()
		TArray< UTeam* > teams;

	UPROPERTY()
		TArray< UPlayerRecord* > players;

	UPROPERTY()
		bool requestingAddPlayer = false;

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;

	UFUNCTION()
		virtual bool update(float deltaTime);

	virtual void beginPlay();

	virtual ULocalPlayer* getLocalPlayerOrCreateNew(int controllerID, bool isGamePad);
	virtual ULocalPlayer* getLocalPlayer(int controllerID);

	virtual void setGameConfiguration(UGameConfiguration* baseGameConfiguration);

	UFUNCTION(BlueprintCallable)
		static void setGameConfigurationByKey(FString gameConfigurationKey);

	virtual FString getMapKey();

	UFUNCTION(BlueprintCallable)
		static FString getMapKeyStatic();

};