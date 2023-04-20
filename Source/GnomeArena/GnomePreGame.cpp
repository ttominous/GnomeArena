#include "GnomePreGame.h"
#include "DualAxisCreature.h"
#include "ComplexGameState.h"
#include "PlayerRecord.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"


void UGnomePreGame::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState || !gameState->gameConfiguration) {
		return;
	}

	APlayerController* controller = GetOwningPlayer();
	bool scoreboardActive = controller->IsInputKeyDown(FKey("Tab")) || controller->IsInputKeyDown(FKey("Gamepad_Special_Left"));
	UUITools::setVisibility(this, "Scoreboard", scoreboardActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);


	UUITools::setTextBlockText(this, "GameTitle", gameState->gameConfiguration->displayName);
	UUITools::setTextBlockText(this, "GameDescription", gameState->gameConfiguration->description);
	UUITools::setTextBlockText(this, "GameTimer", FString::FromInt(FGenericPlatformMath::CeilToInt(gameState->gameStartDelay)));

}