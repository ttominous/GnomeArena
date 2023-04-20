#include "DebugTextWidget.h"
#include "UITools.h"
#include "SettingsTools.h"
#include "HTTPTools.h"
#include "ResourceCache.h"


UDebugTextWidget* UDebugTextWidget::instance = nullptr;
TArray< FDebugTextEntry > UDebugTextWidget::debugTexts;
TQueue< FDebugTextEntry > UDebugTextWidget::incomingDebugTexts;

bool UDebugTextWidget::Initialize() {
	if (instance == nullptr) {
		instance = this;
	}
	return Super::Initialize();
}

void UDebugTextWidget::BeginDestroy() {
	if (instance == this) {
		instance = nullptr;
	}
	Super::BeginDestroy();
}

void UDebugTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {

	Super::NativeTick(MyGeometry, InDeltaTime);
	if (dead) {
		return;
	}

	float deltaTime = FApp::GetDeltaTime();
	instance = this;

	int framesPerSecond = USettingsTools::getFramesPerSecond(GetWorld());
	UUITools::setTextBlockText(this, "FPS", "FPS: " + FString::FromInt(framesPerSecond));
	if (framesPerSecond > 40) {
		UUITools::setTextBlockColor(this, "FPS", FColor::Green);
	}
	else if (framesPerSecond > 20) {
		UUITools::setTextBlockColor(this, "FPS", FColor::Yellow);
	}
	else {
		UUITools::setTextBlockColor(this, "FPS", FColor::Red);
	}

	UUITools::setTextBlockText(this, "IPAddress", "IP: " + UHTTPTools::getExternalIP());



	FDebugTextEntry debugText;
	while (incomingDebugTexts.Dequeue(debugText)) {

		if (debugText.time <= 0.6f) {
			int lastIndex = debugTexts.Num() - 1;
			if (lastIndex >= 0 && debugTexts[lastIndex].text.Equals(debugText.text) && debugTexts[lastIndex].color == debugText.color && debugTexts[lastIndex].time <= 0.6f) {
				debugTexts[lastIndex].time = debugText.time;
				debugTexts[lastIndex].drawnOnce = false;
				continue;
			}
		}
		debugTexts.Add(debugText);
	}

	for (int i = 0; i < debugTexts.Num(); i++) {
		debugTexts[i].time -= deltaTime;
		if (debugTexts[i].time <= 0.0f) {
			if (!debugTexts[i].drawnOnce) {
				debugTexts[i].drawnOnce = true;
			}
			else {
				debugTexts.RemoveAt(i);
				i--;
			}
		}
	}

	UVerticalBox* container = UUITools::getVerticalBox(this, "Container");
	if (!container) {
		return;
	}

	if (entryClass == nullptr) {
		entryClass = UResourceCache::getWidgetClass("DebugTextEntry");
	}
	if (!entryClass) {
		return;
	}

	UUITools::syncVerticalBoxChildren(container, debugTexts.Num(), entryClass);
	TArray< UWidget* > 	widgets = container->GetAllChildren();
	for (int i = 0; i < debugTexts.Num(); i++) {
		UUserWidget* widget = Cast< UUserWidget >(widgets[i]);
		UUITools::setTextBlockText(widget, "TextValue", debugTexts[i].text);
		UUITools::setTextBlockColor(widget, "TextValue", debugTexts[i].color);
	}
}


void UDebugTextWidget::addText(FString text, FColor color, float time) {
	FDebugTextEntry debugText;
	debugText.text = text;
	debugText.color = color;
	debugText.time = time;
	incomingDebugTexts.Enqueue(debugText);
}