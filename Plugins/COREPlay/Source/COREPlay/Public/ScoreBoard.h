#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Team.h"
#include "PlayerRecord.h"

#include "ScoreBoard.generated.h"

UCLASS()
class COREPLAY_API UScoreBoard : public UUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		int maxTeams = 2;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > playerEntryClass = nullptr;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void updateTeam(int teamIndex, UTeam* team);
	virtual void updatePlayer(int teamIndex, UTeam* team, int playerIndex, UPlayerRecord* player, UUserWidget* widget);

};
