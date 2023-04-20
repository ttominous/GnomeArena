#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"

#include "ComplexViewportClient.generated.h"

UCLASS()
class COREPLAY_API UComplexViewportClient : public UGameViewportClient {
	GENERATED_BODY()

public:

	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual bool InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;

};
