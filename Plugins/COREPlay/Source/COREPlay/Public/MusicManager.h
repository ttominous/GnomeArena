#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"

#include "MusicManager.generated.h"


UCLASS()
class COREPLAY_API AMapMusic : public AActor {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString musicCueKey;

	AMapMusic();

};



UCLASS()
class COREPLAY_API UMusicManager : public UObject {

	GENERATED_BODY()

public:

	static UMusicManager* instance;

	UPROPERTY()
		FString currentMusicKey;

	UPROPERTY()
		UAudioComponent* musicComponent = nullptr;

	virtual void handleChangeLevel();

};