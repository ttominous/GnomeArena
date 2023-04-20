#pragma once

#include "CoreMinimal.h"
#include "Payload.h"
#include "Item.h"

#include "ItemSpec.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UItemSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		int value = 0;

	UPROPERTY(EditAnywhere)
		int weight = 0;

	UPROPERTY(EditAnywhere)
		int durability = 0;

	UPROPERTY(EditAnywhere)
		bool stackable = true;

	UPROPERTY(EditAnywhere)
		int maxStack = 0;

	UPROPERTY(EditAnywhere)
		int defaultDurability = 0;

	UPROPERTY(EditAnywhere)
		int defaultQuantity = 1;

	UPROPERTY(EditAnywhere)
		UTexture2D* iconTexture = nullptr;

	UPROPERTY(EditAnywhere)
		UStaticMesh* dropItemMesh = nullptr;

	UPROPERTY(EditAnywhere)
		FTransform dropItemTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UItem > itemClass = UItem::StaticClass();

};
