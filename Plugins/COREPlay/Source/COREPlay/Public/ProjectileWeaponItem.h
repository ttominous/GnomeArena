#pragma once

#include "CoreMinimal.h"
#include "Item.h"

#include "ProjectileWeaponItem.generated.h"


UCLASS()
class COREPLAY_API UProjectileWeaponItem : public UItem {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated)
		FString ammoItemKey;

	UPROPERTY(EditAnywhere, replicated)
		int clipCount = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UProjectileWeaponItem, ammoItemKey);
		DOREPLIFETIME(UProjectileWeaponItem, clipCount);
	}

	virtual void copyTo(UItem* otherItem) override;
	virtual void initNew() override;

};