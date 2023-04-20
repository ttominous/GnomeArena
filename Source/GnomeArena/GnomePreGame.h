#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GnomePreGame.generated.h"

UCLASS()
class GNOMEARENA_API UGnomePreGame : public UUserWidget {

	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
