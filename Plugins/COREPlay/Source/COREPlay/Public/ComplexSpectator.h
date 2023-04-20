#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "ComplexSpectator.generated.h"


UCLASS()
class COREPLAY_API AStaticCameraPoint : public AActor {
	GENERATED_BODY()

public:

	static TArray< AStaticCameraPoint* > instances;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


};

UCLASS()
class COREPLAY_API AComplexSpectatorPawn : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		UCameraComponent* camera = nullptr;

	AComplexSpectatorPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void init();

};


UCLASS()
class COREPLAY_API ASpectateMapPawn : public AComplexSpectatorPawn
{
	GENERATED_BODY()

public:

	static ASpectateMapPawn* spawn(UWorld* world, FVector location, FRotator direction);
	virtual void init() override;
};
