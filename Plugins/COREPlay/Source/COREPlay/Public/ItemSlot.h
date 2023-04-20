#pragma once

#include "CoreMinimal.h"

#include "ItemSlot.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FItemSlot {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString itemKey;

	UPROPERTY(EditAnywhere)
		int itemIndex = -1;

	UPROPERTY(EditAnywhere, NotReplicated)
		FString temporaryItemKey;

};