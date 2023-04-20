#include "VirtualKeyboardWidget.h"
#include "ResourceCache.h"
#include "ComplexPlayerController.h"
#include "ComplexHUD.h"
#include "UITools.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"


void UVirtualKeyboardWidget::init() {

	if (buttonClass) {

		UUserWidget* acceptButton = UUITools::getUserWidget(this, "AcceptButton");
		if (acceptButton == nullptr) {
			return;
		}

		TArray< FString > characters;
		characters.Add("A");
		characters.Add("B");
		characters.Add("C");
		characters.Add("D");
		characters.Add("E");
		characters.Add("F");
		characters.Add("G");
		characters.Add("H");
		characters.Add("I");
		characters.Add("J");
		characters.Add("K");
		characters.Add("L");
		characters.Add("M");
		characters.Add("N");
		characters.Add("O");
		characters.Add("P");
		characters.Add("Q");
		characters.Add("R");
		characters.Add("S");
		characters.Add("T");
		characters.Add("U");
		characters.Add("V");
		characters.Add("W");
		characters.Add("X");
		characters.Add("Y");
		characters.Add("Z");
		characters.Add("a");
		characters.Add("b");
		characters.Add("c");
		characters.Add("d");
		characters.Add("e");
		characters.Add("f");
		characters.Add("g");
		characters.Add("h");
		characters.Add("i");
		characters.Add("j");
		characters.Add("k");
		characters.Add("l");
		characters.Add("m");
		characters.Add("n");
		characters.Add("o");
		characters.Add("p");
		characters.Add("q");
		characters.Add("r");
		characters.Add("s");
		characters.Add("t");
		characters.Add("u");
		characters.Add("v");
		characters.Add("w");
		characters.Add("x");
		characters.Add("y");
		characters.Add("1");
		characters.Add("2");
		characters.Add("3");
		characters.Add("3");
		characters.Add("4");
		characters.Add("5");
		characters.Add("6");
		characters.Add("7");
		characters.Add("8");
		characters.Add("9");
		characters.Add("0");
		characters.Add(" ");
		characters.Add("-");
		characters.Add("_");
		characters.Add("<--");

		UUniformGridPanel* buttonGrid = UUITools::getUniformGridPanel(this, "ButtonGrid");
		if (buttonGrid) {

			int width = 20;
			int height = FGenericPlatformMath::CeilToInt((double)characters.Num() / (double)width);

			UUITools::syncUniformGridChildren(buttonGrid, characters.Num(), width, buttonClass);
			TArray< UWidget* > 	buttonEntries = buttonGrid->GetAllChildren();
			
			int x = 0;
			int y = 0;
			for (int i = 0; i < characters.Num(); i++) {
				UComplexButton* button = Cast< UComplexButton >(buttonEntries[i]);
				button->displayText = characters[i];

				if (i == 0) {
					defaultGamePadFocus = button;
					UUITools::setNavigationVertical(acceptButton, button);
				}

				int j = i + width;
				if (j < characters.Num() - 1) {
					UComplexButton* nextButton = Cast< UComplexButton >(buttonEntries[j]);
					UUITools::setNavigationVertical(button, nextButton);
				}
				else {
					if (x == 0) {
						UUITools::setNavigationVertical(button, acceptButton);
					}
					else {
						UUITools::setNavigationDown(button, acceptButton);
					}
				}

				if (x == width - 1 || i == characters.Num() - 1) {
					int k = y * width;
					UComplexButton* firstButton = Cast< UComplexButton >(buttonEntries[k]);
					UUITools::setNavigationHorizontal(button, firstButton);
				}
				if( x > 0) {
					int k = y * width + (x-1);
					UComplexButton* previousButton = Cast< UComplexButton >(buttonEntries[k]);
					UUITools::setNavigationHorizontal(previousButton, button);
				}

				button->onSelectDelegate.AddDynamic(this, &UVirtualKeyboardWidget::selectCharacter);


				x++;
				if (x >= width) {
					x = 0;
					y++;
				}
			}
		}
	}

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}
	Super::init();
}



void UVirtualKeyboardWidget::handleGamePadUseChange(bool usingGamepad) {
	if (!usingGamepad) {
		close();
		return;
	}
	Super::handleGamePadUseChange(usingGamepad);
}

void UVirtualKeyboardWidget::cancel() {
	if (onCancel.IsBound()) {
		onCancel.Broadcast(this);
	}
	close();
}

void UVirtualKeyboardWidget::finish() {
	if (onFinish.IsBound()) {
		onFinish.Broadcast(this, currentTextValue);
	}
	close();
}

void UVirtualKeyboardWidget::setValue(FString value) {
	currentTextValue = value;
	UUITools::setTextBlockText(this, "CurrentValue", currentTextValue);
}

void UVirtualKeyboardWidget::setTitle(FString value) {
	UUITools::setTextBlockText(this, "Title", value);
}

void UVirtualKeyboardWidget::selectCharacter(UUserWidget* keyWidget) {

	UComplexButton* button = Cast< UComplexButton >(keyWidget);
	FString value = button->displayText;

	if (value.Equals("<--")) {
		setValue(currentTextValue.Mid(0,currentTextValue.Len()-1));
		return;
	}
	setValue(currentTextValue + value);
}