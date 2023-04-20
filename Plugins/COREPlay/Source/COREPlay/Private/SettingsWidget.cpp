#include "SettingsWidget.h"
#include "SettingsTools.h"
#include "CommonWidgets.h"
#include "Debug.h"

void USettingsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USettingsWidget::init() {
	Super::init();

	commonLowToHighValues.Add("Potato");
	commonLowToHighValues.Add("Low");
	commonLowToHighValues.Add("Medium");
	commonLowToHighValues.Add("High");
	commonLowToHighValues.Add("Ultra");

	refreshSettings();
}

void USettingsWidget::onBooleanChange(UUserWidget* instigator, bool newValue) {
	FString key = instigator->GetName();
	
	if (key.Equals("UseVsync")) {
		USettingsTools::setUseVSync(newValue);
		refreshApplyChanges();
	}
}

void USettingsWidget::onNumericChange(UUserWidget* instigator, double newValue) {
	FString key = instigator->GetName();

	if (key.Equals("RenderScale")) {
		USettingsTools::setRenderScale(newValue);
		refreshApplyChanges();
	}
}

void USettingsWidget::onStringChange(UUserWidget* instigator, FString newValue) {
	FString key = instigator->GetName();

	if (key.Equals("Fullscreen")) {
		EWindowMode::Type fullscreenType = EWindowMode::Type::Fullscreen;
		if (newValue.Equals("Windowed")) {
			fullscreenType = EWindowMode::Type::Windowed;
		}
		else if (newValue.Equals("Windowed Fullscreen")) {
			fullscreenType = EWindowMode::Type::WindowedFullscreen;
		}
		USettingsTools::setFullscreenType(fullscreenType);
		refreshApplyChanges();
	}
	else if (key.Equals("ShadowQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setShadowQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("ShadingQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setShadingQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("TextureQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setTextureQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("ViewRangeQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setViewRangeQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("FoliageQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setFoliageQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("PostProcessQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setPostProcessQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("VisualEffectQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setVisualEffectQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("AntiAliasingQuality")) {
		int quality = commonLowToHighValues.Find(newValue);
		USettingsTools::setAntiAliasingQuality(quality);
		refreshApplyChanges();
	}
	else if (key.Equals("WindowSize")) {
		TMap<FString, FIntPoint> options = USettingsTools::getPossibleScreenResolutions();
		FIntPoint resolution = options[newValue];
		USettingsTools::setScreenResolution(resolution);
		refreshApplyChanges();
	}
}

void USettingsWidget::saveChanges() {
	USettingsTools::applySettings();
	USettingsTools::settingsChanged = false;
	refreshApplyChanges();
}

void USettingsWidget::cancelUnappliedChanges() {
	USettingsTools::loadGameSettings(GetWorld()->WorldType != EWorldType::PIE);
	USettingsTools::settingsChanged = false;
}

void USettingsWidget::refreshSettings() {

	UArrowSelect* windowSize = Cast< UArrowSelect >(UUITools::getWidget(this, "WindowSize"));
	if (windowSize) {
		TArray<FString> options = USettingsTools::getPossibleScreenResolutionKeys();
		FIntPoint currentResolution = USettingsTools::getScreenResolution();
		FString currentResolutionKey = FString::FromInt(currentResolution.X) + "x" + FString::FromInt(currentResolution.Y);

		windowSize->setValuesFromArray(options);
		windowSize->setValueFromFinalValue(options[options.Find(currentResolutionKey)]);
		windowSize->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		windowSize->init();
	}

	UBooleanInput* useVsync = Cast< UBooleanInput >(UUITools::getWidget(this, "UseVsync"));
	if (useVsync) {
		useVsync->setValue(USettingsTools::getUseVSync());
		useVsync->onValueChanged.AddDynamic(this, &USettingsWidget::onBooleanChange);
	}

	UArrowSelect* fullscreen = Cast< UArrowSelect >(UUITools::getWidget(this, "Fullscreen"));
	if (fullscreen) {
		TArray< FString > values;
		values.Add("Fullscreen");
		values.Add("Windowed");
		values.Add("Windowed Fullscreen");

		EWindowMode::Type fullscreenType = USettingsTools::getFullscreenType();
		FString currentValue = "Fullscreen";
		if (fullscreenType == EWindowMode::Type::Windowed) {
			currentValue = "Windowed";
		}
		else if (fullscreenType == EWindowMode::Type::WindowedFullscreen) {
			currentValue = "Windowed Fullscreen";
		}

		fullscreen->setValuesFromArray(values);
		fullscreen->setValueFromFinalValue(currentValue);
		fullscreen->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		fullscreen->init();
	}

	UArrowSelect* shadowQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "ShadowQuality"));
	if (shadowQuality) {
		int quality = USettingsTools::getShadowQuality();
		shadowQuality->setValuesFromArray(commonLowToHighValues);
		shadowQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		shadowQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		shadowQuality->init();
	}

	UArrowSelect* shadingQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "ShadingQuality"));
	if (shadingQuality) {
		int quality = USettingsTools::getShadingQuality();
		shadingQuality->setValuesFromArray(commonLowToHighValues);
		shadingQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		shadingQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		shadingQuality->init();
	}

	UArrowSelect* textureQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "TextureQuality"));
	if (textureQuality) {
		int quality = USettingsTools::getTextureQuality();
		textureQuality->setValuesFromArray(commonLowToHighValues);
		textureQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		textureQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		textureQuality->init();
	}

	UArrowSelect* viewRangeQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "ViewRangeQuality"));
	if (viewRangeQuality) {
		int quality = USettingsTools::getViewRangeQuality();
		viewRangeQuality->setValuesFromArray(commonLowToHighValues);
		viewRangeQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		viewRangeQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		viewRangeQuality->init();
	}

	UArrowSelect* foliageQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "FoliageQuality"));
	if (foliageQuality) {
		int quality = USettingsTools::getFoliageQuality();
		foliageQuality->setValuesFromArray(commonLowToHighValues);
		foliageQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		foliageQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		foliageQuality->init();
	}

	UArrowSelect* postProcessQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "PostProcessQuality"));
	if (postProcessQuality) {
		int quality = USettingsTools::getPostProcessQuality();
		postProcessQuality->setValuesFromArray(commonLowToHighValues);
		postProcessQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		postProcessQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		postProcessQuality->init();
	}

	UArrowSelect* visualEffectQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "VisualEffectQuality"));
	if (visualEffectQuality) {
		int quality = USettingsTools::getVisualEffectQuality();
		visualEffectQuality->setValuesFromArray(commonLowToHighValues);
		visualEffectQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		visualEffectQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		visualEffectQuality->init();
	}

	UArrowSelect* antiAliasingQuality = Cast< UArrowSelect >(UUITools::getWidget(this, "AntiAliasingQuality"));
	if (antiAliasingQuality) {
		int quality = USettingsTools::getAntiAliasingQuality();
		antiAliasingQuality->setValuesFromArray(commonLowToHighValues);
		antiAliasingQuality->setValueFromFinalValue(commonLowToHighValues[quality]);
		antiAliasingQuality->onValueChanged.AddDynamic(this, &USettingsWidget::onStringChange);
		antiAliasingQuality->init();
	}

	USliderBar* renderScale = Cast< USliderBar >(UUITools::getWidget(this, "RenderScale"));
	if (renderScale) {
		float value = USettingsTools::getRenderScale();
		renderScale->init();
		renderScale->setValue(value);
		renderScale->onValueChanged.AddDynamic(this, &USettingsWidget::onNumericChange);
	}

	USettingsTools::settingsChanged = false;
	refreshApplyChanges();
}

void USettingsWidget::refreshApplyChanges() {
	UComplexUserWidget* applyChanges = Cast< UComplexUserWidget >(UUITools::getWidget(this, "ApplyChanges"));
	if (applyChanges) {
		applyChanges->setDisabled(!USettingsTools::settingsChanged);
	}
}