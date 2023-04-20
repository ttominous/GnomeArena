#include "ScoreBoard.h"
#include "ComplexGameState.h"
#include "Creature.h"
#include "Team.h"
#include "PlayerRecord.h"
#include "ResourceCache.h"
#include "UITools.h"
#include "MathTools.h"
#include "ControllerTools.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"


void UScoreBoard::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState || !gameState->gameConfiguration) {
		return;
	}

	UUITools::setTextBlockText(this, "GameTitle", gameState->gameConfiguration->displayName);

	for (int i = 0; i < maxTeams; i++) {
		updateTeam(i, gameState->teams.Num() > i ? gameState->teams[i] : nullptr);
	}

}

void UScoreBoard::updateTeam(int teamIndex, UTeam* team) {
	FString teamKey = "Team" + FString::FromInt((teamIndex + 1));

	if (team == nullptr) {
		UUITools::setVisibility(this, teamKey, ESlateVisibility::Collapsed);
	}
	else {

		FLinearColor rowColor = team->color;
		rowColor.R *= 0.5f;
		rowColor.G *= 0.5f;
		rowColor.B *= 0.5f;
		rowColor.A = 0.5f;

		UUITools::setTextBlockText(this, teamKey + "_Name", team->displayName);
		UUITools::setBrushColor(this, teamKey + "_Header", FSlateColor(rowColor));
		UUITools::setTextBlockText(this, teamKey + "_Score", FString::FromInt(team->kills));

		UVerticalBox* playerContainer = UUITools::getVerticalBox(this, teamKey + "_Players");
		if (!playerContainer || playerEntryClass == nullptr) {
			return;
		}

		TArray<UPlayerRecord*> players = team->players;
		Algo::SortBy(players, &UPlayerRecord::score, TGreater<>());

		UUITools::syncVerticalBoxChildren(playerContainer, team->players.Num(), playerEntryClass);

		TArray< UWidget* > 	playerEntries = playerContainer->GetAllChildren();
		for (int i = 0; i < playerEntries.Num(); i++) {
			UUserWidget* playerWidget = Cast< UUserWidget >(playerEntries[i]);
			if (playerWidget && playerEntries.Num() > i) {
				updatePlayer(teamIndex, team, i, players[i], playerWidget);
			}
		}


		UUITools::setVisibility(this, teamKey, ESlateVisibility::Visible);
	}
}

void UScoreBoard::updatePlayer(int teamIndex, UTeam* team, int playerIndex, UPlayerRecord* player, UUserWidget* widget) {

	FLinearColor rowColor = team->color;
	if (playerIndex % 2 == 0) {
		rowColor.R *= 0.35f;
		rowColor.G *= 0.35f;
		rowColor.B *= 0.35f;
	}
	else{
		rowColor.R *= 0.25f;
		rowColor.G *= 0.25f;
		rowColor.B *= 0.25f;
	}
	rowColor.A = 0.5f;

	UUITools::setBrushColor(widget, "Background", FSlateColor(rowColor));
	UUITools::setTextBlockText(widget, "Name", player->displayName);
	UUITools::setTextBlockText(widget, "Score", FString::FromInt(player->score));
	UUITools::setTextBlockText(widget, "Kills", FString::FromInt(player->kills));
	UUITools::setTextBlockText(widget, "Deaths", FString::FromInt(player->deaths));

	if (player->userProfile) {
		int localPlayerIndex = UControllerTools::getLocalPlayerIndex(Cast<APlayerController>(player->controller)) + 1;
		UUITools::setTextBlockText(widget, "PlayerIndex", FString::FromInt(localPlayerIndex));

		UUITools::setVisibility(widget, "OnlineProfile", ESlateVisibility::Collapsed);
		UUITools::setVisibility(widget, "PlayerIndex", ESlateVisibility::Visible);

		UUITools::setTextBlockColor(widget, "Name", FSlateColor(FColor::Yellow));
		UUITools::setTextBlockColor(widget, "Score", FSlateColor(FColor::Yellow));
		UUITools::setTextBlockColor(widget, "Kills", FSlateColor(FColor::Yellow));
		UUITools::setTextBlockColor(widget, "Deaths", FSlateColor(FColor::Yellow));
		UUITools::setTextBlockColor(widget, "PlayerIndex", FSlateColor(FColor::Yellow));
	}
	else {
		UUITools::setVisibility(widget, "OnlineProfile", player->isBot ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		UUITools::setVisibility(widget, "PlayerIndex", ESlateVisibility::Collapsed);

		UUITools::setTextBlockColor(widget, "Name", FSlateColor(FColor::White));
		UUITools::setTextBlockColor(widget, "Score", FSlateColor(FColor::White));
		UUITools::setTextBlockColor(widget, "Kills", FSlateColor(FColor::White));
		UUITools::setTextBlockColor(widget, "Deaths", FSlateColor(FColor::White));
		UUITools::setBrushColor(widget, "OnlineProfile", FSlateColor(FColor::White));

	}

}