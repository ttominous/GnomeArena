#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "Item.generated.h"


class UItemSpec;

UCLASS()
class COREPLAY_API UItem : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated)
		FString itemSpecKey;

	UPROPERTY()
		UItemSpec* itemSpec = nullptr;

	UPROPERTY(EditAnywhere, replicated)
		int quantity = 1;

	UPROPERTY(EditAnywhere, replicated)
		int durability = 1;

	virtual bool IsSupportedForNetworking() const override {
		return true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UItem, itemSpecKey);
		DOREPLIFETIME(UItem, quantity);
		DOREPLIFETIME(UItem, durability);
	}

	virtual UItemSpec* getItemSpec();
	virtual void copyTo(UItem* otherItem);
	virtual void initNew();
};