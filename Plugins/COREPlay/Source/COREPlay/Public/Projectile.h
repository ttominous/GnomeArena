#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "TargetDetail.h"
#include "DamageDetails.h"

#include "Projectile.generated.h"

class UProjectileSpec;

UCLASS()
class COREPLAY_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated)
		bool ready = false;

	UPROPERTY()
		bool spawned = false;

	UPROPERTY(BlueprintReadWrite)
		bool collided = false;

	UPROPERTY(BlueprintReadWrite, replicated)
		FVector direction;

	UPROPERTY(BlueprintReadWrite)
		FVector spawnLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated)
		float speed = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated)
		float lifeSpan = 2.0f;

	UPROPERTY(BlueprintReadWrite, replicated)
		float life = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool killOnCollide = true;

	UPROPERTY()
		FTargetDetail attacker;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated)
		UProjectileSpec* spec = nullptr;

	AProjectile();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual bool spawn();
	virtual void deSpawn();
	virtual void update(float deltaTime);
	virtual void handleCollision(FHitResult hit);

	UFUNCTION(NetMulticast, Reliable)
		void sendWorldCollision(FHitResult hit);
	virtual void handleWorldCollision(FHitResult hit);

	UFUNCTION(BlueprintImplementableEvent)
		void onSpawn();

	UFUNCTION(BlueprintImplementableEvent)
		void onDeSpawn();

	UFUNCTION(BlueprintImplementableEvent)
		void onCollide(FHitResult hit);


	static AProjectile* spawn(UObject* owner, UWorld* world, UProjectileSpec* projectileSpec, FVector location, FRotator direction, float speedModifier);
};
