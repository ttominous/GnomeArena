#pragma once

#include "CoreMinimal.h"
#include "UITools.h"

#include "RenderTargetPassOverlayWidget.generated.h"

UCLASS()
class COREPLAY_API URenderTargetPassOverlayWidget : public UUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY()
		TMap< FString, UImage* > images;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};