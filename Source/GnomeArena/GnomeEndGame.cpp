#include "GnomeEndGame.h"
#include "DualAxisCreature.h"
#include "ComplexGameState.h"
#include "ComplexGameInstance.h"
#include "PlayerRecord.h"
#include "ResourceCache.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"

#include "Kismet/GameplayStatics.h"


void UGnomeEndGame::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState || !gameState->gameConfiguration) {
		return;
	}

	APlayerController* controller = GetOwningPlayer();
	bool scoreboardActive = controller->IsInputKeyDown(FKey("Tab")) || controller->IsInputKeyDown(FKey("Gamepad_Special_Left"));
	UUITools::setVisibility(this, "Scoreboard", scoreboardActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (gameInstance && gameInstance->returningToMainMenu) {
		UUITools::setVisibility(this, "MainContainer", ESlateVisibility::Hidden);
		return;
	}
	UUITools::setVisibility(this, "MainContainer", ESlateVisibility::Visible);

	UUITools::setTextBlockText(this, "GameTitle", gameState->gameConfiguration->displayName);
	UUITools::setTextBlockText(this, "GameTimer", FString::FromInt(FGenericPlatformMath::CeilToInt(gameState->gameEndDelay)));

	updateScoreBoard(InDeltaTime);
}



void UGnomeEndGame::updateScoreBoard(float deltaTime) {

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	UTeam* owningTeam = UTeam::get(GetOwningPlayer());
	UTeam* winningTeam = nullptr;

	if (gameState->teams.Num() >= 1) {
		UTeam* team1 = gameState->teams[0];
		FLinearColor teamColor1 = FLinearColor(team1->color);
		teamColor1.A = 0.5f;
		UUITools::setBrushColor(this, "ScoreA_TeamColor1", FSlateColor(teamColor1));
		UUITools::setTextBlockText(this, "ScoreA_TeamScore1", FString::FromInt(team1->kills));

		UUITools::setVisibility(this, "ScoreA_Team1", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "ScoreA_Team1", ESlateVisibility::Collapsed);
	}

	if (gameState->teams.Num() >= 2) {
		UTeam* team2 = gameState->teams[1];
		FLinearColor teamColor2 = FLinearColor(team2->color);
		teamColor2.A = 0.5f;
		UUITools::setBrushColor(this, "ScoreA_TeamColor2", FSlateColor(teamColor2));
		UUITools::setTextBlockText(this, "ScoreA_TeamScore2", FString::FromInt(team2->kills));

		UUITools::setVisibility(this, "ScoreA_Team2", ESlateVisibility::Visible);

		if (gameState->teams[0]->kills > gameState->teams[1]->kills) {
			winningTeam = gameState->teams[0];
		}
		else {
			winningTeam = gameState->teams[1];
		}
	}
	else {
		UUITools::setVisibility(this, "ScoreA_Team2", ESlateVisibility::Collapsed);
	}

	if (owningTeam != winningTeam) {
		UUITools::setTextBlockText(this, "MatchResult", "YOU LOSE");
		UUITools::setTextBlockColor(this, "MatchResult", FColor(255, 55, 55));

		if (!sounded) {
			UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("Victory"), 1.0f, 1.0f, 0.0f);
			sounded = true;
		}

	}
	else {
		UUITools::setTextBlockText(this, "MatchResult", "YOU WIN");
		UUITools::setTextBlockColor(this, "MatchResult", FColor(55, 255, 55));

		if (!sounded) {
			UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("Defeated"), 1.0f, 1.0f, 0.0f);
			sounded = true;
		}
	}

}