#include "BranchingWidget.h"
#include "ComplexPlayerController.h"
#include "ComplexHUD.h"
#include "Debug.h"

void UBranchingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (currentWidget == nullptr && startingWidgetClass != nullptr) {
		if (getWidgetContainer() == nullptr) {
			return;
		}
		previousWidgetClasses.Empty();
		previousWidgetData.Empty();
		proceedToNext(startingWidgetClass,FPayload());
	}

}

UBorder* UBranchingWidget::getWidgetContainer() {
	if (widgetContainer == nullptr) {
		widgetContainer = UUITools::getBorder(this, "Container");
	}
	return widgetContainer;
}

void UBranchingWidget::proceedToNext(UClass* nextClass, FPayload previousData) {
	if (getWidgetContainer() == nullptr) {
		return;
	}

	if (currentWidget != nullptr) {
		previousWidgetClasses.Add(currentWidgetClass);
		previousWidgetData.Add(previousData);
	}

	currentWidgetClass = nextClass;
	currentWidget = CreateWidget<UUserWidget>(widgetContainer, currentWidgetClass);
	widgetContainer->SetContent(currentWidget);

	UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(currentWidget);
	if (complexWidget) {
		defaultGamePadFocus = complexWidget->defaultGamePadFocus;
	}
	else {
		defaultGamePadFocus = nullptr;
	}

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}

}

void UBranchingWidget::backToPrevious() {
	if (getWidgetContainer() == nullptr || previousWidgetClasses.Num() == 0) {
		return;
	}

	int i = previousWidgetClasses.Num() - 1;
	UClass* nextClass = previousWidgetClasses[i];
	FPayload nextPayload = previousWidgetData[i];
	previousWidgetClasses.RemoveAt(i);
	previousWidgetData.RemoveAt(i);

	currentWidgetClass = nextClass;
	currentWidget = CreateWidget<UUserWidget>(widgetContainer, currentWidgetClass);
	widgetContainer->SetContent(currentWidget);

	UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(currentWidget);
	if (complexWidget) {
		defaultGamePadFocus = complexWidget->defaultGamePadFocus;
	}
	else {
		defaultGamePadFocus = nullptr;
	}

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}
}


void UBranchingWidget::proceedToNextFromChild(UUserWidget* child, UClass* nextClass, FPayload previousData) {
	UBranchingWidget* branchingWidget = Cast<UBranchingWidget>(UUITools::getParentUserWidget(child));
	if (branchingWidget) {
		branchingWidget->proceedToNext(nextClass, previousData);
	}
}

void UBranchingWidget::backToPreviousFromChild(UUserWidget* child) {
	UBranchingWidget* branchingWidget = Cast<UBranchingWidget>(UUITools::getParentUserWidget(child));
	if (branchingWidget) {
		branchingWidget->backToPrevious();
	}
}