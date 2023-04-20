#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ViewportTools.generated.h"

UCLASS()
class COREPLAY_API UViewportTools : public UObject {

	GENERATED_BODY()

public:

	static FIntPoint getViewportSize(APlayerController* controller);
	static FIntPoint getViewportSize(AActor* actor);

};
