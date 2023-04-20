#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Payload.h"

#include "ItemSet.generated.h"

USTRUCT(BlueprintType)
struct COREPLAY_API FItemSetItem {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString itemSpecKey;

	UPROPERTY(EditAnywhere)
		int quantity = 1;

	UPROPERTY(EditAnywhere)
		int durability = 0;

	UPROPERTY(EditAnywhere)
		FString equippedSlot;

	UPROPERTY(EditAnywhere)
		bool equipped = false;

	UPROPERTY(EditAnywhere)
		FPayload additionalData;

};

USTRUCT(BlueprintType)
struct COREPLAY_API FItemSet {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TArray< FItemSetItem > items;

};