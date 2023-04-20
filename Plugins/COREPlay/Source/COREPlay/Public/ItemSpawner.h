#pragma once

#include "CoreMinimal.h"
#include "PointOfInterest.h"
#include "DropItem.h"
#include "ItemSet.h"

#include "ItemSpawner.generated.h"

UCLASS()
class COREPLAY_API AItemSpawner : public APointOfInterest {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf< ADropItem > dropItemClass = nullptr;

	UPROPERTY(EditAnywhere)
		float dropFrequency = 60.0f;

	UPROPERTY(EditAnywhere)
		float dropTime = 60.0f;

	UPROPERTY(EditAnywhere)
		EDropItemType type;

	UPROPERTY(EditAnywhere)
		FString pickupFXListKey;

	UPROPERTY(EditAnywhere)
		FString displaySpecKey;

	UPROPERTY(EditAnywhere)
		FItemSet itemSet;

	UPROPERTY(EditAnywhere)
		TMap< FString, int > stats;


	UPROPERTY()
		ADropItem* currentItem = nullptr;

	AItemSpawner();
	virtual void Tick(float DeltaTime) override;

	virtual FString getType() override;

	static TArray< AItemSpawner* > getAll();

};