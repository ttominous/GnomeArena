#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageDetails.h"

#include "PlayerHUD.generated.h"

USTRUCT()
struct COREPLAY_API FPlayerHUDScoreAlert {


	GENERATED_BODY()

public:

	UPROPERTY()
		FColor instigatorColor;

	UPROPERTY()
		FString instigatorName;

	UPROPERTY()
		FColor targetColor;

	UPROPERTY()
		FString targetName;

	UPROPERTY()
		FString message;

	UPROPERTY()
		UTexture2D* icon = nullptr;

	UPROPERTY()
		float timeLeft;

};


UCLASS()
class COREPLAY_API UPlayerHUD : public UUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		TArray< FPlayerHUDScoreAlert > allDeathAlerts;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > deathAlertWidgetClass = nullptr;

	UPROPERTY(EditAnywhere)
		float allDeathAlertTime = 6.0f;

	UPROPERTY(EditAnywhere)
		FColor defaultNoTeamColor = FColor::White;

	UPROPERTY(EditAnywhere)
		UTexture2D* defaultKillIcon = nullptr;

	UPROPERTY(EditAnywhere)
		int maxAllDeathAlertsToDisplay = 5;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void updateDeathAlerts(float deltaTime);

	virtual void populateAlertVictim(FPlayerHUDScoreAlert& alert, FTargetDetail& target);
	virtual void populateAlertAttacker(FPlayerHUDScoreAlert& alert, FTargetDetail& target);
	virtual void populateKillAlertDescription(FPlayerHUDScoreAlert& alert, FDamageDetails& damage);
	virtual void alertKill(FDamageDetails damage);
	virtual void alertToTarget(FTargetDetail target, FString message, FColor color, FString iconKey);
	virtual void alert(FString message, FColor color, FString iconKey);

};
