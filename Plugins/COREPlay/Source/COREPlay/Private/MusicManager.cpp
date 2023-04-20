#include "MusicManager.h"
#include "ResourceCache.h"

#include "Kismet/GameplayStatics.h"

UMusicManager* UMusicManager::instance = nullptr;

AMapMusic::AMapMusic() {
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
}

void UMusicManager::handleChangeLevel() {

	TArray<AActor*> musicActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMapMusic::StaticClass(), musicActors);

	if (musicActors.Num() > 0) {
		AMapMusic* music = Cast< AMapMusic >(musicActors[0]);

		if (!music->musicCueKey.Equals(currentMusicKey)) {
			USoundBase* sound = UResourceCache::getSound(music->musicCueKey);
			if (sound) {
				currentMusicKey = music->musicCueKey;
				if (musicComponent) {
					musicComponent->Stop();
				}
				musicComponent = UGameplayStatics::SpawnSound2D(GetWorld(), sound, 1.0f, 1.0f, 0.0f, nullptr, true, false);
			}
		}
	}

}