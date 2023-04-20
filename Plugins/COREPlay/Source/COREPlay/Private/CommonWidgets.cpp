#include "CommonWidgets.h"
#include "ResourceCache.h"
#include "ComplexPlayerController.h"
#include "ComplexHUD.h"
#include "VirtualKeyboardWidget.h"
#include "UITools.h"
#include "Debug.h"


void UComplexButton::applyStyle() {
	Super::applyStyle();

	if (displayText.Len() > 0) {
		UUITools::setTextBlockText(this, "TextDisplay", displayText);

		if (displayTextStyleColors.Contains(currentStyle)) {
			UUITools::setTextBlockColor(this, "TextDisplay", displayTextStyleColors[currentStyle]);
		}
		else {
			UUITools::setTextBlockColor(this, "TextDisplay", displayTextColor);
		}

		UUITools::setTextBlockFontSize(this, "TextDisplay", displayTextSize);
		UUITools::setVisibility(this, "TextDisplay", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "TextDisplay", ESlateVisibility::Collapsed);
	}

	if (displayIcon != nullptr) {
		UUITools::setVisibility(this, "IconDisplay", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "IconDisplay", ESlateVisibility::Collapsed);
	}

	if (backgroundStyleColors.Contains(currentStyle)) {
		UUITools::setBrushColor(this, "Background", backgroundStyleColors[currentStyle]);
	}
	else {
		UUITools::setBrushColor(this, "Background", displayBackgroundColor);
	}

	USpacer* size = UUITools::getSpacer(this,"Size");
	if (size) {
		size->SetSize(minSize);
	}
}



void ULabeledInput::setLabel(FString label) {
	labelValue = label;
	UUITools::setTextBlockText(this, "Label", labelValue);
	UUITools::setVisibility(this, "Label", labelValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ULabeledInput::init() {
	Super::init();
	setLabel(labelValue);
}




void USliderBar::setMinLabel(FString inMinLabel) {
	minLabelValue = inMinLabel;
	UUITools::setTextBlockText(this, "MinLabel", minLabelValue);
	UUITools::setVisibility(this, "MinLabel", minLabelValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USliderBar::setMaxLabel(FString inMaxLabel) {
	maxLabelValue = inMaxLabel;
	UUITools::setTextBlockText(this, "MaxLabel", maxLabelValue);
	UUITools::setVisibility(this, "MaxLabel", maxLabelValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void USliderBar::setMin(float inMin) {
	updating = true;
	min = inMin;
	USlider* slider = UUITools::getSlider(this, "SliderInput");
	if (slider) {
		slider->SetMinValue(min);
	}
	updating = false;
}

void USliderBar::setMax(float inMax) {
	updating = true;
	max = inMax;
	USlider* slider = UUITools::getSlider(this, "SliderInput");
	if (slider) {
		slider->SetMaxValue(max);
	}
	updating = false;
}

void USliderBar::setValue(float inValue) {
	if (updating) {
		return;
	}
	updating = true;
	currentValue = inValue;


	USlider* slider = UUITools::getSlider(this,"SliderInput");
	if (slider) {
		slider->SetValue(currentValue);
	}

	FString displayValue = getDisplayValue();
	UUITools::setTextBlockText(this, "ValueLabel", displayValue);
	UUITools::setVisibility(this, "ValueLabel", displayValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	updating = false;
	if (onValueChanged.IsBound()) {
		onValueChanged.Broadcast(this, currentValue);
	}
}


void USliderBar::setStep(float inStepSize) {
	updating = true;
	stepSize = inStepSize;
	USlider* slider = UUITools::getSlider(this, "SliderInput");
	if (slider) {
		slider->SetStepSize(stepSize);
	}
	updating = false;
}

FString USliderBar::getDisplayValue() {
	return FString::SanitizeFloat(currentValue);
}

void USliderBar::init() {
	Super::init();

	updating = true;

	USlider* slider = UUITools::getSlider(this, "SliderInput");
	if (slider) {
		slider->SetValue(currentValue);
	}
	FString displayValue = getDisplayValue();
	UUITools::setTextBlockText(this, "ValueLabel", displayValue);
	UUITools::setVisibility(this, "ValueLabel", displayValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	setMax(max);
	updating = true;

	setMin(min);
	updating = true;

	setStep(stepSize);
	updating = true;

	setMinLabel(minLabelValue);
	setMaxLabel(maxLabelValue);

	updating = false;
}


void USliderBar::handleInputPress(const FKey Key) {
	if (!disabled && canInteract()) {
		FString keyName = Key.GetFName().ToString();
		onInputPress(keyName);
		if (keyName.Equals("Gamepad_RightStick_Left")) {
			setValue(currentValue - stepSize);
			return;
		}
		if (keyName.Equals("Gamepad_RightStick_Right")) {
			setValue(currentValue + stepSize);
			return;
		}
	}
	Super::handleInputPress(Key);
}




void UArrowSelect::setValue(FString inValue) {

	if (updating) {
		return;
	}
	updating = true;

	if (values.Contains(inValue)) {
		value = inValue;
		UUITools::setTextBlockText(this, "ValueLabel", value);
	}

	updating = false;
	if (onValueChanged.IsBound()) {
		onValueChanged.Broadcast(this, getFinalValue());
	}
}

void UArrowSelect::setValueFromFinalValue(FString inValue) {
	for (auto& entry : values) {
		if (entry.Value.Equals(inValue)) {
			setValue(entry.Key);
			return;
		}
	}
}

void UArrowSelect::setValues(TMap< FString, FString > inValues) {
	values = inValues;
	if (!values.Contains(value)) {
		TArray<FString> keys;
		values.GetKeys(keys);
		if (keys.Num() > 0) {
			setValue(keys[0]);
		}
		else {
			setValue("");
		}
	}
}

void UArrowSelect::setValuesFromArray(TArray< FString > inValues) {
	TMap< FString, FString > newValues;
	for (FString inValue : inValues) {
		newValues.Add(inValue, inValue);
	}
	setValues(newValues);
}

FString UArrowSelect::getFinalValue() {
	if (values.Contains(value)) {
		return values[value];
	}
	return "";
}

void UArrowSelect::init() {
	Super::init();
	updating = true;
	UUITools::setTextBlockText(this, "ValueLabel", value);
	updating = false;
}

void UArrowSelect::nextValue() {
	TArray<FString> keys;
	values.GetKeys(keys);
	int index = keys.Find(value);

	if (index > -1) {
		index++;
		if (index >= keys.Num()) {
			index = 0;
		}
		setValue(keys[index]);
	}
	else if (keys.Num() > 0) {
		setValue(keys[0]);
	}
}

void UArrowSelect::previousValue() {
	TArray<FString> keys;
	values.GetKeys(keys);
	int index = keys.Find(value);

	if (index > -1) {
		index--;
		if (index < 0) {
			index = keys.Num() -1;
		}
		setValue(keys[index]);
	}
	else if (keys.Num() > 0) {
		setValue(keys[0]);
	}
}

void UArrowSelect::handleInputPress(const FKey Key) {
	if (!disabled && canInteract()) {
		FString keyName = Key.GetFName().ToString();
		onInputPress(keyName);
		if (keyName.Equals("Gamepad_RightStick_Left")) {
			previousValue();
			return;
		}
		if (keyName.Equals("Gamepad_RightStick_Right")) {
			nextValue();
			return;
		}
	}
	Super::handleInputPress(Key);
}




void UBooleanInput::setValue(bool inValue) {
	updating = true;
	value = inValue;
	updating = false;
	if (onValueChanged.IsBound()) {
		onValueChanged.Broadcast(this, value);
	}
	styleDirty = true;
}

bool UBooleanInput::getValue() {
	return value;
}

void UBooleanInput::toggleValue() {
	if (!disabled) {
		setValue(!value);
	}
}







void UTextBox::setValue(FString inValue) {

	if (updating) {
		return;
	}
	updating = true;
	value = inValue;
	UUITools::setEditableTextValue(this, "ValueField", value);
	updating = false;
	if (onValueChanged.IsBound()) {
		onValueChanged.Broadcast(this, value);
	}
}

void UTextBox::init() {
	Super::init();
	updating = true;
	UUITools::setEditableTextValue(this, "ValueField", value);
	updating = false;
}

void UTextBox::handleSelect() {
	Super::handleSelect();

	if (!canInteract() || disabled) {
		return;
	}

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		UVirtualKeyboardWidget* keyboard = Cast< UVirtualKeyboardWidget >( hud->openVirtualInput("VirtualKeyboard") );
		if (keyboard) {
			keyboard->setTitle("Enter " + (labelValue.Len() == 0 ? "a Value" : labelValue));
			keyboard->setValue(value);
			keyboard->onFinish.AddDynamic(this, &UTextBox::finishEditingFromVirtualKeyboard);
		}
	}
}

void UTextBox::finishEditingFromVirtualKeyboard(UUserWidget* virtualKeyboard, FString newValue) {
	setValue(newValue);
}




void UColorPicker::setColor(FColor color) {
	currentColor = color;
	refreshPreview();
}

void UColorPicker::refreshPreview() {
	UUITools::setBrushColor(this, "ColorPreview", FSlateColor(currentColor));
}

void UColorPicker::startEditing() {

	if (disabled) {
		return;
	}

	UColorPickerPopup* popup = UColorPickerPopup::open(popupLayerKey, popupWidgetKey, popupZIndex, GetOwningPlayer(), popupWidgetClass, currentColor, popupTitle, popupTitleColor, popupMessage, popupMessageColor);
	if (popup) {
		popup->onFinish.AddDynamic(this, &UColorPicker::finishEditing);
	}
}

void UColorPicker::finishEditing(UUserWidget* instigator, FColor color) {
	currentColor = color;
	refreshPreview();
	if (onColorChanged.IsBound()) {
		onColorChanged.Broadcast(this, currentColor);
	}
}


void UColorPicker::init() {
	Super::init();
	updating = true;
	setColor(currentColor);
	updating = false;
}





void UPopUp::init() {
	Super::init();
	UUITools::setTextBlockText(this, "Title", titleValue);
	UUITools::setTextBlockColor(this, "Title", FSlateColor(titleColorValue));
	UUITools::setTextBlockText(this, "Message", messageValue);
	UUITools::setTextBlockColor(this, "Message", FSlateColor(messageColorValue));
	UUITools::setVisibility(this, "Message", messageValue.Len() > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}






void UColorPickerPopup::updateRed(float value) {
	if (updating) {
		return;
	}
	value = FMath::Clamp(value, 0.0f, 1.0f);
	currentColor.R = 255 * value;
	refreshPreview();
}

void UColorPickerPopup::updateGreen(float value) {
	if (updating) {
		return;
	}
	value = FMath::Clamp(value, 0.0f, 1.0f);
	currentColor.G = 255 * value;
	refreshPreview();
}

void UColorPickerPopup::updateBlue(float value) {
	if (updating) {
		return;
	}
	value = FMath::Clamp(value, 0.0f, 1.0f);
	currentColor.B = 255 * value;
	refreshPreview();
}

void UColorPickerPopup::refreshPreview() {
	UUITools::setBrushColor(this, "ColorPreview", FSlateColor(currentColor));
}

void UColorPickerPopup::finish() {
	if (onFinish.IsBound()) {
		onFinish.Broadcast(this, currentColor);
	}
	close();
}

void UColorPickerPopup::cancel() {
	if (onCancel.IsBound()) {
		onCancel.Broadcast(this);
	}
	close();
}

void UColorPickerPopup::init() {
	Super::init();
	updating = true;
	refreshPreview();

	USliderBar* redSlider = Cast<USliderBar>(UUITools::getUserWidget(this, "RedSlider"));
	if (redSlider) {
		redSlider->setValue( (float)currentColor.R / 255.0f);
	}

	USliderBar* greenSlider = Cast<USliderBar>(UUITools::getUserWidget(this, "GreenSlider"));
	if (greenSlider) {
		greenSlider->setValue((float)currentColor.G / 255.0f);
	}

	USliderBar* blueSlider = Cast<USliderBar>(UUITools::getUserWidget(this, "BlueSlider"));
	if (blueSlider) {
		blueSlider->setValue((float)currentColor.B / 255.0f);
	}

	updating = false;
}

UColorPickerPopup* UColorPickerPopup::open(FString layerKey, FString widgetKey, int zLayer, APlayerController* playerController, UClass* popupClass, FColor initialColor, FString title, FColor titleColor, FString message, FColor messageColor) {
	AComplexHUD* hud = AComplexHUD::get(playerController);
	if (hud) {
		UColorPickerPopup* popup = Cast< UColorPickerPopup >( hud->addWidget(layerKey,widgetKey,popupClass, zLayer, true) );
		if (popup) {
			popup->titleValue = title;
			popup->titleColorValue = titleColor;
			popup->messageValue = message;
			popup->messageColorValue = messageColor;
			popup->currentColor = initialColor;
			popup->init();
		}
		return popup;
	}
	return nullptr;
}