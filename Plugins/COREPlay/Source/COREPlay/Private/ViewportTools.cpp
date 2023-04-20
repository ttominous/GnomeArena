#include "ViewportTools.h"

FIntPoint UViewportTools::getViewportSize(APlayerController* controller) {

	if (!controller || !controller->IsLocalController()) {
		return FIntPoint(1, 1);
	}

	FIntPoint screenSize;
	controller->GetViewportSize(screenSize.X, screenSize.Y);

	return screenSize;
}

FIntPoint UViewportTools::getViewportSize(AActor* actor) {
	if (!actor || !actor->IsA(APawn::StaticClass())) {
		return FIntPoint(1, 1);
	}
	return getViewportSize(Cast<APlayerController>(((APawn*)actor)->Controller));
}