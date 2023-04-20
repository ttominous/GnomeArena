#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "DecalSpec.h"

#include "WorldDecal.generated.h"

UCLASS()
class COREPLAY_API AWorldDecal : public AActor {
	GENERATED_BODY()

public:

	UPROPERTY()
		float life = -1.0f;

	UPROPERTY()
		float maxLife = -1.0f;

	UPROPERTY()
		UDecalComponent* decalComponent = nullptr;

	AWorldDecal();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void setMaxLife(float MaxLife);

	static AWorldDecal* spawnWorldDecal(UWorld* world, FVector location, FRotator rotation, UDecalSpec* decalSpec);
};