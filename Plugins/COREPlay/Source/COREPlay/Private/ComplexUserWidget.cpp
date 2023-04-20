#include "ComplexUserWidget.h"
#include "ComplexPlayerController.h"
#include "ComplexHUD.h"
#include "ControllerTools.h"
#include "UITools.h"
#include "Debug.h"


void UComplexUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!initialized) {
		initialized = true;
		init();
		onInit();
	}

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	if (!controller) {
		return;
	}
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (!hud) {
		return;
	}

	bool canHover = canInteract(hud);
	bool hasHover = canHover && ( (!controller->usingGamepad && IsHovered()) || (controller->usingGamepad && hud->virtualHoverWidget == this) );
	if (hasHover) {
		hud->mouseHoveredWidget = this;
	}
	else if (hud->mouseHoveredWidget == this) {
		hud->mouseHoveredWidget = nullptr;
	}

	if (hasHover != hovered) {
		hovered = hasHover;
		styleDirty = true;
	}

	if (styleDirty) {
		styleDirty = false;
		currentStyle = getStyle();
		applyStyle();
		onApplyStyle(currentStyle);
	}

}

void UComplexUserWidget::init() {
	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	if (controller) {
		handleGamePadUseChange(controller->usingGamepad);
	}
}

EWidgetStyleState UComplexUserWidget::getStyle() {
	if (disabled) {
		return hovered ? EWidgetStyleState::DISABLED_HOVERED : EWidgetStyleState::DISABLED;
	}
	if (selected) {
		return hovered ? EWidgetStyleState::SELECTED_HOVERED : EWidgetStyleState::SELECTED;
	}
	return hovered ? EWidgetStyleState::HOVERED : EWidgetStyleState::DEFAULT;
}

void UComplexUserWidget::applyStyle() {

}


void UComplexUserWidget::handleGamePadUseChange(bool usingGamepad) {
	TArray< UWidget* > children = UUITools::getAllChildren(this);
	for (UWidget* child : children) {
		UComplexUserWidget* complexWidget = Cast< UComplexUserWidget >(child);
		if (complexWidget) {
			complexWidget->styleDirty = true;
			complexWidget->onHandleGamePadUseChange(usingGamepad);
		}
	}
	styleDirty = true;
	onHandleGamePadUseChange(usingGamepad);
}

void UComplexUserWidget::handleInputPress(const FKey Key) {

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	if (!controller) {
		return;
	}
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (!hud) {
		return;
	}

	FString keyName = Key.GetFName().ToString();
	onInputPress(keyName);

	//UDebug::print("Pressed " + keyName);

	if ((keyName.Equals("Gamepad_DPad_Up") || keyName.Equals("Gamepad_LeftStick_Up")) && Navigation) {
		UUserWidget* widget = Cast< UUserWidget >( Navigation->Up.Widget.Get() );
		if (widget) {
			hud->setVirtualFocus(widget);
		}
	}
	else if ((keyName.Equals("Gamepad_DPad_Down") || keyName.Equals("Gamepad_LeftStick_Down")) && Navigation) {
		UUserWidget* widget = Cast< UUserWidget >(Navigation->Down.Widget.Get());
		if (widget) {
			hud->setVirtualFocus(widget);
		}
	}
	else if ((keyName.Equals("Gamepad_DPad_Left") || keyName.Equals("Gamepad_LeftStick_Left")) && Navigation) {
		UUserWidget* widget = Cast< UUserWidget >(Navigation->Left.Widget.Get());
		if (widget) {
			hud->setVirtualFocus(widget);
		}
	}
	else if ((keyName.Equals("Gamepad_DPad_Right") || keyName.Equals("Gamepad_LeftStick_Right")) && Navigation) {
		UUserWidget* widget = Cast< UUserWidget >(Navigation->Right.Widget.Get());
		if (widget) {
			hud->setVirtualFocus(widget);
		}
	}
	else if (keyName.Equals("Gamepad_FaceButton_Bottom")) {
		handleSelect();
	}
}

void UComplexUserWidget::handleInputRelease(const FKey Key) {
	FString keyName = Key.GetFName().ToString();
	onInputRelease(keyName);
}



void UComplexUserWidget::handleSelect() {
	if (!canInteract() || disabled) {
		return;
	}
	onHandleSelect();
	if (onSelectDelegate.IsBound()) {
		onSelectDelegate.Broadcast(this);
	}
}

void UComplexUserWidget::triggerSelect() {
	handleSelect();
}

bool UComplexUserWidget::canInteract() {
	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	if (!controller) {
		return false;
	}
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (!hud) {
		return false;
	}
	return canInteract(hud);
}

bool UComplexUserWidget::canInteract(AComplexHUD* hud) {
	if (hud->currentFocusedWidget == nullptr ) {
		return true;
	}
	if (path.Len() == 0) {
		path = UUITools::getPath(this);
	}
	if (!hud->interactableCache.Contains(path)) {
		bool interactable = UUITools::isChildOf(hud->currentFocusedWidget, this);
		hud->interactableCache.Add(path, interactable);
	}
	return hud->interactableCache[path];
}

void UComplexUserWidget::close() {
	AComplexHUD* hud = AComplexHUD::get(GetOwningPlayer());
	if (hud) {
		hud->removeWidget(this);
	}
}

void UComplexUserWidget::setSelected(bool isSelected) {
	if (selected != isSelected) {
		selected = isSelected;
		styleDirty = true;
	}
}

void UComplexUserWidget::setDisabled(bool isDisabled) {
	if (disabled != isDisabled) {
		disabled = isDisabled;
		styleDirty = true;
	}
}

bool UComplexUserWidget::isDisabled() {
	return disabled;
}
