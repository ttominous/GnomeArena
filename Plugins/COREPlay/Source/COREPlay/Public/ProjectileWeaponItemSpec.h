#pragma once

#include "CoreMinimal.h"
#include "WeaponItemSpec.h"
#include "PlayAnimationData.h"
#include "ProjectileBurst.h"
#include "FXList.h"

#include "ProjectileWeaponItemSpec.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UProjectileWeaponItemSpec : public UWeaponItemSpec {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FProjectileBurst > projectiles;

	UPROPERTY(EditAnywhere)
		TMap< FString, UFXList* > effects;

	UPROPERTY(EditAnywhere)
		TArray< FString > ammoItemKeys;

	UPROPERTY(EditAnywhere)
		bool pumpAfterShooting = false;

	UPROPERTY(EditAnywhere)
		bool pumpAfterReloading = false;

	UPROPERTY(EditAnywhere)
		int clipSize = 1;

	UPROPERTY(EditAnywhere)
		int reloadAmount = 1;

	UPROPERTY(EditAnywhere)
		int defaultClipCount = 0;

	UPROPERTY(EditAnywhere)
		FString defaultAmmoItemSpecKey;
};
