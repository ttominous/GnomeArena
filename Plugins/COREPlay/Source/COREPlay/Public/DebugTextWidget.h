#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DebugTextWidget.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FDebugTextEntry {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString text;

	UPROPERTY()
		float time;

	UPROPERTY()
		FColor color;

	UPROPERTY()
		bool drawnOnce = false;

};

UCLASS()
class COREPLAY_API UDebugTextWidget : public UUserWidget {

	GENERATED_BODY()

public:

	static UDebugTextWidget* instance;
	static TArray< FDebugTextEntry > debugTexts;
	static TQueue< FDebugTextEntry > incomingDebugTexts;

	UPROPERTY()
		bool dead = false;

	UPROPERTY(EditAnywhere)
		TSubclassOf< UUserWidget > entryClass = nullptr;

	virtual bool Initialize() override;
	virtual void BeginDestroy() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	static void addText(FString text, FColor color, float time);
};