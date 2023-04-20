#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DualAxisCreatureHUD.generated.h"

class ADualAxisCreature;

UCLASS()
class COREPLAY_API UDualAxisCreatureHUD : public UUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		float aimOffset = 90.0f;

	UPROPERTY()
		float targetAimOffset = 25.0f;

	UPROPERTY()
		float scoreChangeTimer = 0.0f;

	UPROPERTY()
		int lastScore = 0;

	UPROPERTY()
		int scoreChangeBuffer = 0;

	UPROPERTY()
		bool skipScoreChange = true;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void updateReticle(ADualAxisCreature* creature, float deltaTime);
	virtual void updateStats(ADualAxisCreature* creature, float deltaTime);
	virtual void updateAmmo(ADualAxisCreature* creature, float deltaTime);
	virtual void updateScoreChange(ADualAxisCreature* creature, float deltaTime);
};
