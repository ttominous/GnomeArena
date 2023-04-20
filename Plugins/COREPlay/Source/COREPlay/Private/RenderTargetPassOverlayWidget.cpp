#include "RenderTargetPassOverlayWidget.h"
#include "ComplexSceneCaptureComponent2D.h"
#include "UITools.h"
#include "Debug.h"

void URenderTargetPassOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* controller = GetOwningPlayer();
	if (!controller || !controller->GetPawn()) {
		return;
	}


	TArray< UComplexSceneCaptureComponent2D* > captureComponents = UComplexSceneCaptureComponent2D::getAll(GetOwningPlayer()->GetPawn());
	if (captureComponents.Num() == 0) {
		return;
	}

	UOverlay* container = UUITools::getOverlay(this, "Container");
	if (!container) {
		return;
	}

	for (UComplexSceneCaptureComponent2D* captureComponent : captureComponents) {
		if (!images.Contains(captureComponent->passKey)) {

			UImage* image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			image->SetBrushFromMaterial(captureComponent->material);

			UOverlaySlot* slot = container->AddChildToOverlay(image);
			slot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			images.Add(captureComponent->passKey, image);

		}
		images[captureComponent->passKey]->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}