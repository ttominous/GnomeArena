#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"

#include "RayTracedProjectile.generated.h"

UCLASS()
class COREPLAY_API ARayTracedProjectile : public AProjectile
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FVector lastPosition;

	UPROPERTY(EditAnywhere)
		bool debugDraw = false;

public:

	virtual bool spawn() override;
	virtual void update(float deltaTime) override;

};
