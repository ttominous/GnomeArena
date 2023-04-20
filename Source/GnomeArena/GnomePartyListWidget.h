#pragma once

#include "CoreMinimal.h"
#include "ComplexUserWidget.h"

#include "UITools.h"

#include "GnomePartyListWidget.generated.h"


UCLASS()
class GNOMEARENA_API UGnomePartyListWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > profileEntryClass = nullptr;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};