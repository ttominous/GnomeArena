#include "ComplexGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"
#include "ComplexGameState.h"
#include "SettingsTools.h"
#include "GameTools.h"

#include "EngineSettings/Classes/GameMapsSettings.h"


#include "Debug.h"


bool UComplexGameInstance::isShuttingDown = false;

void UComplexGameInstance::Init() {
	UComplexGameInstance::isShuttingDown = false;
	Super::Init();
	tickDelegate = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UComplexGameInstance::update));

	UGameTools::reset();

	if (resourceCacheClass == nullptr && resourceCache != nullptr) {
		resourceCache = DuplicateObject<UResourceCache>(resourceCache, this);
	}
	else {
		if (!resourceCacheClass) {
			resourceCacheClass = UResourceCache::StaticClass();
		}
		resourceCache = NewObject< UResourceCache >(this, resourceCacheClass);
	}
	UResourceCache::instance = resourceCache;


	if (saveManagerClass == nullptr && saveManager != nullptr) {
		saveManager = DuplicateObject<USaveManager>(saveManager, this);
	}
	else {
		if (!saveManagerClass) {
			saveManagerClass = USaveManager::StaticClass();
		}
		saveManager = NewObject< USaveManager >(this, saveManagerClass);
	}
	USaveManager::instance = saveManager;
	if (saveManager != nullptr) {
		saveManager->init();
	}


	if (httpManagerClass == nullptr && httpManager != nullptr) {
		httpManager = DuplicateObject<UHTTPManager>(httpManager, this);
	}
	else {
		if (!httpManagerClass) {
			httpManagerClass = UHTTPManager::StaticClass();
		}
		httpManager = NewObject< UHTTPManager >(this, httpManagerClass);
	}
	UHTTPManager::instance = httpManager;


	if (userManagerClass == nullptr && userManager != nullptr) {
		userManager = DuplicateObject<UUserManager>(userManager, this);
	}
	else {
		if (!userManagerClass) {
			userManagerClass = UUserManager::StaticClass();
		}
		userManager = NewObject< UUserManager >(this, userManagerClass);
	}
	UUserManager::instance = userManager;


	if (musicManagerClass == nullptr && musicManager != nullptr) {
		musicManager = DuplicateObject<UMusicManager>(musicManager, this);
	}
	else {
		if (!musicManagerClass) {
			musicManagerClass = UMusicManager::StaticClass();
		}
		musicManager = NewObject< UMusicManager >(this, musicManagerClass);
	}
	UMusicManager::instance = musicManager;
}

void UComplexGameInstance::Shutdown() {
	UComplexGameInstance::isShuttingDown = true;
	UResourceCache::instance = nullptr;
	USaveManager::instance = nullptr;
	UHTTPManager::instance = nullptr;
	UMusicManager::instance = nullptr;
	FTSTicker::GetCoreTicker().RemoveTicker(tickDelegate);

	UDebug::log("GameInstance has shutdown.");
	Super::Shutdown();
}

void UComplexGameInstance::StartGameInstance() {
	Super::StartGameInstance();
	UDebug::log("GameInstance has started.");
}



bool UComplexGameInstance::update(float deltaTime) {

	if (requestingAddPlayer) {
		if (LocalPlayers.Num() < 4) {
			FString error;
			this->GetGameViewportClient()->MaxSplitscreenPlayers = 4;
			this->GetGameViewportClient()->SetForceDisableSplitscreen(false);
			ULocalPlayer* localPlayer = CreateLocalPlayer(LocalPlayers.Num(), error, true);
		}
		requestingAddPlayer = false;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (gameState) {
		gameState->update(deltaTime);
	}
	if (httpManager) {
		httpManager->update(deltaTime);
	}
	return true;
}

void UComplexGameInstance::beginPlay() {
	if (firstBeginPlay) {
		USettingsTools::init();
		if (GetWorld()->WorldType == EWorldType::PIE || GetWorld()->WorldType == EWorldType::Editor){
			USettingsTools::loadGameSettings(false);
		}
		else {
			USettingsTools::loadGameSettings(true);
			USettingsTools::applySettings();
		}

		UUserManager::instance->loadProfiles();

		firstBeginPlay = false;
	}

	if (musicManager) {
		musicManager->handleChangeLevel();
	}
}

ULocalPlayer* UComplexGameInstance::getLocalPlayerOrCreateNew(int controllerID, bool isGamePad) {
	int playerIndex = controllerID;
	if (isGamePad && !routeFirstGamePadToPlayerOne) {
		playerIndex++;
	}

	ULocalPlayer* localPlayer = getLocalPlayer(playerIndex);
	if (localPlayer == nullptr) {
		requestingAddPlayer = true;
	}
	return localPlayer;
}

ULocalPlayer* UComplexGameInstance::getLocalPlayer(int controllerID) {
	if (controllerID < 0 || LocalPlayers.Num() <= controllerID) {
		return nullptr;
	}
	return LocalPlayers[controllerID];
}

void UComplexGameInstance::setGameConfiguration(UGameConfiguration* baseGameConfiguration) {
	gameConfiguration = NewObject< UGameConfigurationInstance >(this);
	gameConfiguration->copyFromBase(baseGameConfiguration);
	gameConfiguration->baseConfiguration = baseGameConfiguration;
	teams.Empty();
	players.Empty();

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (gameState) {
		gameState->initGameConfiguration();
	}
}


void UComplexGameInstance::setGameConfigurationByKey(FString gameConfigurationKey) {
	UGameConfiguration* baseGameConfiguration = UResourceCache::getGameConfiguration(gameConfigurationKey);
	if (!baseGameConfiguration) {
		UDebug::error("Could not find GameConfiguration: " + gameConfigurationKey);
		return;
	}
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(UResourceCache::instance->GetWorld()->GetGameInstance());
	if (!gameInstance) {
		UDebug::error("No valid GameInstance! Could not set GameConfiguration: " + gameConfigurationKey);
		return;
	}
	gameInstance->setGameConfiguration(baseGameConfiguration);
}

FString UComplexGameInstance::getMapKey() {
	return gameConfiguration != nullptr ? gameConfiguration->mapKey : "";
}


FString UComplexGameInstance::getMapKeyStatic() {
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(UResourceCache::instance->GetWorld()->GetGameInstance());
	if (!gameInstance) {
		return "";
	}
	return gameInstance->getMapKey();
}