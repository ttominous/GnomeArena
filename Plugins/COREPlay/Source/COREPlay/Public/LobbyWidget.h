#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplexUserWidget.h"
#include "PlayerRecord.h"

#include "LobbyWidget.generated.h"


UCLASS()
class COREPLAY_API ULobbyWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > playerListEntryClass = nullptr;

	virtual void init() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void refreshMapPreview();
	virtual void updatePlayerList();
	virtual void updatePlayer(UUserWidget* widget, UPlayerRecord* player);
	
	UFUNCTION()
		virtual void changeMapSelection(UUserWidget* widget, FString value);
};
