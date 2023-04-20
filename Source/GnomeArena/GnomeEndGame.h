#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GnomeEndGame.generated.h"

UCLASS()
class GNOMEARENA_API UGnomeEndGame : public UUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool sounded = false;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void updateScoreBoard(float deltaTime);

};
