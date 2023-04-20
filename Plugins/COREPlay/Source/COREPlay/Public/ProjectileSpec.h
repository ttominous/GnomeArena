#pragma once

#include "CoreMinimal.h"
#include "DamageDetails.h"
#include "FXList.h"
#include "DecalSpec.h"

#include "ProjectileSpec.generated.h"

class AProjectile;

UCLASS(Blueprintable)
class COREPLAY_API UProjectileSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		FDamageDetails damage;

	UPROPERTY(EditAnywhere)
		float lifeSpan = 2.0f;

	UPROPERTY(EditAnywhere)
		float speed = 1.0f;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AProjectile> projectileClass = nullptr;

	UPROPERTY(EditAnywhere)
		TMap< FString, UFXList* > hitEffects;

	UPROPERTY(EditAnywhere)
		TMap< FString, UDecalSpec* > hitDamageDecals;

};
