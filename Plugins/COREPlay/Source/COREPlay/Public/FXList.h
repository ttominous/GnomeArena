#pragma once

#include "CoreMinimal.h"
#include "ParticleFX.h"
#include "SoundFX.h"

#include "FXList.generated.h"



UCLASS()
class COREPLAY_API UFXList : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FParticleFX > particles;

	UPROPERTY(EditAnywhere)
		TArray< FSoundFX > sounds;

	virtual void execute(AActor* actor, FVector location, float yaw, FVector scale, FName socketName = "", USceneComponent* component = nullptr);
	virtual void execute(AActor* actor, FVector location, FRotator rotation, FVector scale, FName socketName = "", USceneComponent* component = nullptr);

};