#include "LobbyWidget.h"
#include "ComplexGameInstance.h"
#include "GameConfiguration.h"
#include "Creature.h"
#include "Team.h"
#include "PlayerRecord.h"
#include "ResourceCache.h"
#include "UITools.h"
#include "MathTools.h"
#include "ControllerTools.h"
#include "CommonWidgets.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"

void ULobbyWidget::init() {
	Super::init();

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance || !gameInstance->gameConfiguration) {
		initialized = false;
		return;
	}

	UUITools::setTextBlockText(this, "GameTitle", gameInstance->gameConfiguration->displayName);
	UUITools::setTextBlockText(this, "GameDescription", gameInstance->gameConfiguration->description);

	UArrowSelect* mapOptions = Cast< UArrowSelect >(UUITools::getWidget(this, "MapOptions"));
	if (mapOptions) {
		if (gameInstance->gameConfiguration->baseConfiguration) {
			mapOptions->setValues(gameInstance->gameConfiguration->baseConfiguration->mapOptions);
			mapOptions->setValueFromFinalValue(gameInstance->gameConfiguration->mapKey);
		}
		mapOptions->onValueChanged.AddDynamic(this, &ULobbyWidget::changeMapSelection);
	}

	if (GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		if (mapOptions) {
			mapOptions->setDisabled(true);
		}

		UComplexUserWidget* startButton = Cast< UComplexUserWidget >(UUITools::getWidget(this, "Start"));
		if (startButton) {
			startButton->setDisabled(true);
		}
	}



	refreshMapPreview();
}

void ULobbyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState || !gameState->gameConfiguration) {
		return;
	}

	UArrowSelect* mapOptions = Cast< UArrowSelect >(UUITools::getWidget(this, "MapOptions"));
	if (mapOptions && !mapOptions->value.Equals(gameState->gameConfiguration->mapKey)) {
		mapOptions->setValueFromFinalValue(gameState->gameConfiguration->mapKey);
		refreshMapPreview();
	}

	updatePlayerList();
}

void ULobbyWidget::refreshMapPreview() {
	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance || !gameInstance->gameConfiguration) {
		initialized = false;
		return;
	}

	FString textureKey = "mapPreview_" + gameInstance->gameConfiguration->mapKey;
	UUITools::setImageBrushTexture(this, "MapPreview", UResourceCache::getTexture(textureKey));
}

void ULobbyWidget::updatePlayerList() {

	if (playerListEntryClass == nullptr) {
		return;
	}

	UVerticalBox* playerListContainer = UUITools::getVerticalBox(this, "PlayerListContainer");
	if (!playerListContainer) {
		return;
	}

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	UUITools::syncVerticalBoxChildren(playerListContainer, gameState->players.Num(), playerListEntryClass);

	TArray< UWidget* > 	playerEntries = playerListContainer->GetAllChildren();
	for (int i = 0; i < playerEntries.Num(); i++) {
		UUserWidget* playerWidget = Cast< UUserWidget >(playerEntries[i]);

		if (playerWidget && gameState->players.Num() > i) {
			updatePlayer(playerWidget, gameState->players[i]);
		}
	}

}

void ULobbyWidget::updatePlayer(UUserWidget* widget, UPlayerRecord* player) {
	UUITools::setTextBlockText(widget, "Name", player->displayName);

	if (player->userProfile) {
		int playerIndex = UControllerTools::getLocalPlayerIndex(Cast<APlayerController>(player->controller)) + 1;
		UUITools::setTextBlockText(widget, "PlayerIndex", FString::FromInt(playerIndex));

		UUITools::setVisibility(widget, "OnlineProfile", ESlateVisibility::Collapsed);
		UUITools::setVisibility(widget, "PlayerIndex",  ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(widget, "OnlineProfile", ESlateVisibility::Visible);
		UUITools::setVisibility(widget, "PlayerIndex", ESlateVisibility::Collapsed);
	}
}

void ULobbyWidget::changeMapSelection(UUserWidget* widget, FString value) {

	UComplexGameInstance* gameInstance = Cast< UComplexGameInstance >(GetWorld()->GetGameInstance());
	if (!gameInstance || !gameInstance->gameConfiguration) {
		initialized = false;
		return;
	}
	gameInstance->gameConfiguration->mapKey = value;
	refreshMapPreview();
}