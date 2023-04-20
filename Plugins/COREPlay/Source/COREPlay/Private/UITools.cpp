#include "UITools.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/BorderSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/Spacer.h"

#include "Blueprint/SlateBlueprintLibrary.h"

#include "Debug.h"

UWidget* UUITools::getWidget(UUserWidget* parent, FString name) {
	if (parent) {
		UWidget* widget = parent->GetWidgetFromName(*name);
		if (widget == nullptr) {
		}
		return widget;
	}
	return nullptr;
}

UTextBlock* UUITools::getTextBlock(UUserWidget* parent, FString name) {
	return Cast < UTextBlock >(getWidget(parent, name));
}

UScrollBox* UUITools::getScrollBox(UUserWidget* parent, FString name) {
	return Cast < UScrollBox >(getWidget(parent, name));
}

UImage* UUITools::getImage(UUserWidget* parent, FString name) {
	return Cast < UImage >(getWidget(parent, name));
}

UBorder* UUITools::getBorder(UUserWidget* parent, FString name) {
	return Cast < UBorder >(getWidget(parent, name));
}

UCanvasPanel* UUITools::getCanvasPanel(UUserWidget* parent, FString name) {
	return Cast < UCanvasPanel >(getWidget(parent, name));
}

UProgressBar* UUITools::getProgressBar(UUserWidget* parent, FString name) {
	return Cast < UProgressBar >(getWidget(parent, name));
}

UVerticalBox* UUITools::getVerticalBox(UUserWidget* parent, FString name) {
	return Cast < UVerticalBox >(getWidget(parent, name));
}

UHorizontalBox* UUITools::getHorizontalBox(UUserWidget* parent, FString name) {
	return Cast < UHorizontalBox >(getWidget(parent, name));
}

UButton* UUITools::getButton(UUserWidget* parent, FString name) {
	return Cast < UButton >(getWidget(parent, name));
}

UListView* UUITools::getListView(UUserWidget* parent, FString name) {
	return Cast < UListView >(getWidget(parent, name));
}

USlider* UUITools::getSlider(UUserWidget* parent, FString name) {
	return Cast < USlider >(getWidget(parent, name));
}

UCheckBox* UUITools::getCheckBox(UUserWidget* parent, FString name) {
	return Cast < UCheckBox >(getWidget(parent, name));
}

UOverlay* UUITools::getOverlay(UUserWidget* parent, FString name) {
	return Cast < UOverlay >(getWidget(parent, name));
}

UComboBoxString* UUITools::getComboBox(UUserWidget* parent, FString name) {
	return Cast < UComboBoxString >(getWidget(parent, name));
}

UEditableTextBox* UUITools::getEditableTextBox(UUserWidget* parent, FString name) {
	return Cast < UEditableTextBox >(getWidget(parent, name));
}

UEditableText* UUITools::getEditableText(UUserWidget* parent, FString name) {
	return Cast < UEditableText >(getWidget(parent, name));
}


UUniformGridPanel* UUITools::getUniformGridPanel(UUserWidget* parent, FString name) {
	return Cast < UUniformGridPanel >(getWidget(parent, name));
}

USpacer* UUITools::getSpacer(UUserWidget* parent, FString name) {
	return Cast < USpacer >(getWidget(parent, name));
}

UUserWidget* UUITools::getUserWidget(UUserWidget* parent, FString name) {
	return Cast < UUserWidget >(getWidget(parent, name));
}

void UUITools::setBrushColor(UUserWidget* parent, FString name, FSlateColor color) {
	setBrushColor(getWidget(parent, name), color);
}

void UUITools::setBrushColor(UWidget* widget, FSlateColor color) {
	if (widget) {
		if (widget->IsA(UBorder::StaticClass())) {
			((UBorder*)widget)->SetBrushColor(FLinearColor(color.GetSpecifiedColor()));
		}
		else if (widget->IsA(UImage::StaticClass())) {
			((UImage*)widget)->SetBrushTintColor(color);
		}
	}
}

void UUITools::setTextBlockColor(UUserWidget* parent, FString name, FSlateColor color) {
	setTextBlockColor(getTextBlock(parent, name), color);
}

void UUITools::setTextBlockColor(UTextBlock* widget, FSlateColor color) {
	if (widget) {
		widget->SetColorAndOpacity(color);
	}
}

void UUITools::setTextBlockText(UUserWidget* parent, FString name, FString text) {
	setTextBlockText(getTextBlock(parent, name), text);
}

void UUITools::setTextBlockText(UTextBlock* widget, FString text) {
	if (widget) {
		FString currentText = widget->GetText().ToString();
		if (!text.Equals(currentText)) {
			widget->SetText(FText::AsCultureInvariant(text));
		}
	}
}

void UUITools::setTextBlockFontSize(UUserWidget* parent, FString name, int fontSize) {
	setTextBlockFontSize(getTextBlock(parent, name), fontSize);
}

void UUITools::setTextBlockFontSize(UTextBlock* widget, int fontSize) {
	if (widget) {
		FSlateFontInfo font = widget->GetFont();
		font.Size = fontSize;
		widget->SetFont(font);
	}
}

void UUITools::setButtonEnabled(UUserWidget* parent, FString name, bool enabled) {
	setButtonEnabled(getButton(parent, name), enabled);
}

void UUITools::setButtonEnabled(UButton* widget, bool enabled) {
	if (widget && enabled != widget->GetIsEnabled()) {
		widget->SetIsEnabled(enabled);
	}
}

void UUITools::setImageBrushTexture(UUserWidget* parent, FString name, UTexture2D* texture) {
	setImageBrushTexture(getImage(parent, name), texture);
}

void UUITools::setImageBrushTexture(UImage* widget, UTexture2D* texture) {
	if (widget) {
		widget->SetBrushFromTexture(texture);
	}
}

void UUITools::setImageBrushMaterial(UUserWidget* parent, FString name, UMaterialInstance* material) {
	setImageBrushMaterial(getImage(parent, name), material);
}

void UUITools::setImageBrushMaterial(UImage* widget, UMaterialInstance* material) {
	if (widget) {
		widget->SetBrushFromMaterial(material);
	}
}

void UUITools::setProgressBarPerc(UUserWidget* parent, FString name, float perc) {
	setProgressBarPerc(getProgressBar(parent, name), perc);
}

void UUITools::setProgressBarPerc(UProgressBar* widget, float perc) {
	if (widget) {
		widget->SetPercent(perc);
	}
}

void UUITools::setSliderBounds(UUserWidget* parent, FString name, float current, float min, float max, float step) {
	setSliderBounds(getSlider(parent, name), current, min, max, step);
}

void UUITools::setSliderBounds(USlider* widget, float current, float min, float max, float step) {
	if (widget) {
		widget->SetMinValue(min);
		widget->SetMaxValue(max);
		widget->SetStepSize(step);
		widget->SetValue(current);
	}
}

void UUITools::setSliderValue(UUserWidget* parent, FString name, float value) {
	setSliderValue(getSlider(parent, name), value);
}

void UUITools::setSliderValue(USlider* widget, float value) {
	if (widget) {
		widget->SetValue(value);
	}
}

void UUITools::setComboBoxValues(UUserWidget* parent, FString name, TArray< FString > values, FString currentValue) {
	setComboBoxValues(getComboBox(parent, name), values, currentValue);
}

void UUITools::setComboBoxValues(UComboBoxString* widget, TArray< FString > values, FString currentValue) {
	if (widget) {
		widget->ClearOptions();
		for (FString value : values) {
			widget->AddOption(value);
		}
		widget->SetSelectedOption(currentValue);
	}
}

void UUITools::setEditableTextBoxValue(UUserWidget* parent, FString name, FString value) {
	setEditableTextBoxValue(getEditableTextBox(parent, name), value);
}

void UUITools::setEditableTextBoxValue(UEditableTextBox* widget, FString value) {
	if (widget) {
		widget->SetText(FText::AsCultureInvariant(value));
	}
}

void UUITools::setEditableTextValue(UUserWidget* parent, FString name, FString value) {
	setEditableTextValue(getEditableText(parent, name), value);
}

void UUITools::setEditableTextValue(UEditableText* widget, FString value) {
	if (widget) {
		widget->SetText(FText::AsCultureInvariant(value));
	}
}

void UUITools::setVisibility(UUserWidget* parent, FString name, ESlateVisibility visibility) {
	setVisibility(getWidget(parent, name), visibility);
}

void UUITools::setVisibility(UWidget* widget, ESlateVisibility visibility) {
	if (widget) {
		widget->SetVisibility(visibility);
	}
}



float UUITools::getSliderValue(UUserWidget* parent, FString name) {
	return getSliderValue(getSlider(parent, name));
}

float UUITools::getSliderValue(USlider* widget) {
	if (widget) {
		return widget->GetValue();
	}
	return 0.0f;
}

FString UUITools::getComboBoxValue(UUserWidget* parent, FString name) {
	return getComboBoxValue(getComboBox(parent, name));
}

FString UUITools::getComboBoxValue(UComboBoxString* widget) {
	if (widget) {
		return widget->GetSelectedOption();
	}
	return "";
}

FString UUITools::getEditableTextBoxValue(UUserWidget* parent, FString name) {
	return getEditableTextBoxValue(getEditableTextBox(parent, name));
}

FString UUITools::getEditableTextBoxValue(UEditableTextBox* widget) {
	if (widget) {
		return widget->GetText().ToString();
	}
	return "";
}

FString UUITools::getEditableTextValue(UUserWidget* parent, FString name) {
	return getEditableTextValue(getEditableText(parent, name));
}

FString UUITools::getEditableTextValue(UEditableText* widget) {
	if (widget) {
		return widget->GetText().ToString();
	}
	return "";
}

UUserWidget* UUITools::spawnWidget(UClass* widgetClass, UWidget* parent) {
	return spawnWidget("", widgetClass, parent);
}

UUserWidget* UUITools::spawnWidget(FString name, UClass* widgetClass, UWidget* parent) {

	if (!parent) {
		return nullptr;
	}

	if (widgetClass) {

		UUserWidget* widget = nullptr;
		if (!name.Equals("")) {
			widget = CreateWidget<UUserWidget>(parent->GetOwningPlayer(), widgetClass, FName(*name));
		}
		else {
			widget = CreateWidget<UUserWidget>(parent->GetOwningPlayer(), widgetClass);
		}

		if (!widget) {
			return widget;
		}
		if (parent) {

			if (parent->IsA(UCanvasPanel::StaticClass())) {
				((UCanvasPanel*)parent)->AddChildToCanvas(widget);
			}
			else if (parent->IsA(UVerticalBox::StaticClass())) {
				((UVerticalBox*)parent)->AddChildToVerticalBox(widget);
				((UVerticalBoxSlot*)widget->Slot)->SetSize(FSlateChildSize(ESlateSizeRule::Type::Fill));
				((UVerticalBoxSlot*)widget->Slot)->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			}
			else if (parent->IsA(UHorizontalBox::StaticClass())) {
				((UHorizontalBox*)parent)->AddChildToHorizontalBox(widget);
				((UHorizontalBoxSlot*)widget->Slot)->SetSize(FSlateChildSize(ESlateSizeRule::Type::Fill));
				((UHorizontalBoxSlot*)widget->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			}
			else if (parent->IsA(UScrollBox::StaticClass())) {
				((UScrollBox*)parent)->AddChild(widget);
			}
			else if (parent->IsA(UBorder::StaticClass())) {
				((UBorder*)parent)->AddChild(widget);
				((UBorderSlot*)widget->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
				((UBorderSlot*)widget->Slot)->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			}
			else if (parent->IsA(UUniformGridPanel::StaticClass())) {
				UUniformGridPanel* grid = Cast< UUniformGridPanel >(parent);
				grid->AddChildToUniformGrid(widget, 0, 0);
			}

		}

		return widget;

	}
	return nullptr;
}

UUserWidget* UUITools::spawnWidgetToGrid(UClass* widgetClass, int x, int y, UUniformGridPanel* grid) {

	if (!grid) {
		return nullptr;
	}

	if (widgetClass) {

		UUserWidget* widget = CreateWidget<UUserWidget>(grid->GetOwningPlayer(), widgetClass);

		if (!widget) {
			return widget;
		}

		grid->AddChildToUniformGrid(widget, x, y);
		return widget;
	}
	return nullptr;
}

void UUITools::setToolTipText(UUserWidget* parent, FString name, FString text) {
	setToolTipText(getWidget(parent, name), text);
}

void UUITools::setToolTipText(UWidget* widget, FString text) {
	if (widget) {
		widget->SetToolTipText(FText::AsCultureInvariant(text));
	}
}

UUserWidget* UUITools::setToolTipWidgetClass(UUserWidget* parent, FString name, UClass* widgetClass) {
	return setToolTipWidgetClass(getWidget(parent, name), widgetClass);
}

UUserWidget* UUITools::setToolTipWidgetClass(UWidget* widget, UClass* widgetClass) {
	if (widget) {

		UWidget* toolTip = widget->GetToolTip();
		if (toolTip) {
			if (toolTip->IsA(widgetClass)) {
				return Cast<UUserWidget>(toolTip);
			}
			else {
				toolTip->RemoveFromParent();
			}
		}

		UUserWidget* newtoolTip = CreateWidget<UUserWidget>(widget->GetOwningPlayer(), widgetClass);
		widget->SetToolTip(newtoolTip);
		return newtoolTip;
	}
	return nullptr;
}

FVector2D UUITools::getAbsoluteSize(UWidget* widget) {
	const FGeometry geometry = widget->GetCachedGeometry();
	return geometry.GetAbsoluteSize();
}

FVector2D UUITools::getLocalSize(UWidget* widget) {
	const FGeometry geometry = widget->GetCachedGeometry();
	return geometry.GetLocalSize();
}

FVector2D UUITools::getPosition(UWidget* widget, bool inViewportSpace) {
	const FGeometry geometry = widget->GetCachedGeometry();
	FVector2D pos = geometry.GetAbsolutePosition();

	if (inViewportSpace) {
		const FGeometry parentGeometry = getParentUserWidget(widget)->GetCachedGeometry();
		return USlateBlueprintLibrary::AbsoluteToLocal(parentGeometry, pos);
	}
	else {
		return pos;
	}
}

FVector2D UUITools::convertToViewportPos(UWidget* widget, FVector2D pos) {
	UUserWidget* parent = getParentUserWidget(widget);
	if (!parent) {
		parent = Cast< UUserWidget >(widget);
	}
	const FGeometry parentGeometry = parent->GetCachedGeometry();
	return USlateBlueprintLibrary::AbsoluteToLocal(parentGeometry, pos);
}


UUserWidget* UUITools::getParentUserWidget(UWidget* widget) {

	if (widget == nullptr) {
		return nullptr;
	}

	UWidget* parentWidget = widget->GetParent();
	while (parentWidget) {

		UWidget* parentWidgetOriginal = parentWidget;
		parentWidget = parentWidget->GetParent();
		if (!parentWidget) {
			UWidgetTree* widgetTree = Cast< UWidgetTree >(parentWidgetOriginal->GetOuter());
			if (widgetTree) {
				return Cast< UUserWidget >(widgetTree->GetOuter());
			}
		}
	}
	return nullptr;
}

bool UUITools::syncVerticalBoxChildren(UVerticalBox* verticalBox, int numberOfChildren, UClass* childWidgetClass) {

	if (!verticalBox || !childWidgetClass) {
		return false;
	}

	bool changed = false;
	TArray< UWidget* > 	listEntries = verticalBox->GetAllChildren();
	int boxChildrenCount = listEntries.Num();

	for (int p = 0; p < FMath::Max(boxChildrenCount, numberOfChildren); p++) {

		if (p >= listEntries.Num()) {
			UUserWidget* entry = CreateWidget<UUserWidget>(verticalBox, childWidgetClass);
			verticalBox->AddChildToVerticalBox(entry);
			listEntries.Add(entry);
			changed = true;
		}
		else if (p >= numberOfChildren) {
			verticalBox->RemoveChildAt(p);
			changed = true;
		}

	}
	return changed;
}

bool UUITools::syncHorizontalBoxChildren(UHorizontalBox* horizontalBox, int numberOfChildren, UClass* childWidgetClass) {

	if (!horizontalBox || !childWidgetClass) {
		return false;
	}

	TArray< UWidget* > 	listEntries = horizontalBox->GetAllChildren();

	bool changed = false;
	for (int p = 0; p < FMath::Max(listEntries.Num(), numberOfChildren); p++) {

		if (p >= listEntries.Num()) {
			UUserWidget* entry = CreateWidget<UUserWidget>(horizontalBox, childWidgetClass);
			horizontalBox->AddChildToHorizontalBox(entry);
			listEntries.Add(entry);
			changed = true;
		}
		else if (p >= numberOfChildren) {
			horizontalBox->RemoveChildAt(p);
			changed = true;
		}

	}
	return changed;

}

bool UUITools::syncUniformGridChildren(UUniformGridPanel* grid, int numberOfChildren, int width, UClass* childWidgetClass) {
	if (!grid || !childWidgetClass) {
		return false;
	}

	TArray< UWidget* > 	listEntries = grid->GetAllChildren();

	if (numberOfChildren == 0) {
		if (listEntries.Num() > 0) {
			grid->ClearChildren();
			return true;
		}
		return false;
	}

	bool changed = false;
	int x = 0;
	int y = 0;
	for (int p = 0; p < FMath::Max(listEntries.Num(), numberOfChildren); p++) {

		if (p >= listEntries.Num()) {
			UUserWidget* entry = CreateWidget<UUserWidget>(grid, childWidgetClass);
			grid->AddChildToUniformGrid(entry, y, x);
			listEntries.Add(entry);
			changed = true;
		}
		else if (p >= numberOfChildren) {
			grid->RemoveChildAt(p);
			changed = true;
		}
		x++;
		if (x >= width) {
			x = 0;
			y++;
		}

	}
	return changed;
}

void UUITools::setPosition(UWidget* widget, FVector2D position) {
	if (!widget) {
		return;
	}
	setPosition(widget->Slot, position);
}

void UUITools::setPosition(UPanelSlot* slot, FVector2D position) {
	if (!slot) {
		return;
	}
	if (slot->IsA(UCanvasPanelSlot::StaticClass())) {
		UCanvasPanelSlot* canvasPanelSlot = Cast< UCanvasPanelSlot >(slot);
		canvasPanelSlot->SetPosition(position);
	}
}

void UUITools::setSize(UWidget* widget, FVector2D size) {
	if (!widget) {
		return;
	}
	setSize(widget->Slot, size);
}

void UUITools::setSize(UPanelSlot* slot, FVector2D size) {
	if (!slot) {
		return;
	}
	if (slot->IsA(UCanvasPanelSlot::StaticClass())) {
		UCanvasPanelSlot* canvasPanelSlot = Cast< UCanvasPanelSlot >(slot);
		canvasPanelSlot->SetSize(size);
	}
}

TArray< UWidget* > UUITools::getAllChildren(UUserWidget* root) {
	TArray< UWidget* > results;
	getAllChildren(root, results, root);
	return results;
}

void UUITools::getAllChildren(UUserWidget* root, TArray< UWidget* >& collection, UWidget* parent) {
	if (parent->IsA(UUserWidget::StaticClass())) {
		parent = ((UUserWidget*)parent)->GetRootWidget();
	}
	TArray< UWidget* > children;
	root->WidgetTree->GetChildWidgets(parent, children);
	collection.Append(children);
	for (UWidget* child : children) {
		getAllChildren(root, collection, child);
	}
}

bool UUITools::isChildOf(UUserWidget* root, UUserWidget* target) {
	return getAllChildren(root).Contains(target);
}

FString UUITools::getPath(UUserWidget* target) {
	if (target == nullptr) {
		return "";
	}

	FString result;
	if (target->GetParent()) {
		result += getPath( (UUserWidget*)target->GetParent()) + ".";
	}
	result += target->GetName();
	return result;
}


void UUITools::setNavigationUp(UUserWidget* from, UUserWidget* to) {
	if (from == nullptr) {
		return;
	}
	if (from->Navigation == nullptr) {
		from->Navigation = NewObject< UWidgetNavigation >(from);
	}
	if (to == nullptr) {
		from->Navigation->Up.Rule = EUINavigationRule::Stop;
		from->Navigation->Up.Widget = nullptr;
	}
	else {
		from->Navigation->Up.Rule = EUINavigationRule::Explicit;
		from->Navigation->Up.Widget = to;
	}
}

void UUITools::setNavigationDown(UUserWidget* from, UUserWidget* to) {
	if (from == nullptr) {
		return;
	}
	if (from->Navigation == nullptr) {
		from->Navigation = NewObject< UWidgetNavigation >(from);
	}
	if (to == nullptr) {
		from->Navigation->Down.Rule = EUINavigationRule::Stop;
		from->Navigation->Down.Widget = nullptr;
	}
	else {
		from->Navigation->Down.Rule = EUINavigationRule::Explicit;
		from->Navigation->Down.Widget = to;
	}
}

void UUITools::setNavigationLeft(UUserWidget* from, UUserWidget* to) {
	if (from == nullptr) {
		return;
	}
	if (from->Navigation == nullptr) {
		from->Navigation = NewObject< UWidgetNavigation >(from);
	}
	if (to == nullptr) {
		from->Navigation->Left.Rule = EUINavigationRule::Stop;
		from->Navigation->Left.Widget = nullptr;
	}
	else {
		from->Navigation->Left.Rule = EUINavigationRule::Explicit;
		from->Navigation->Left.Widget = to;
	}
}

void UUITools::setNavigationRight(UUserWidget* from, UUserWidget* to) {
	if (from == nullptr) {
		return;
	}
	if (from->Navigation == nullptr) {
		from->Navigation = NewObject< UWidgetNavigation >(from);
	}
	if (to == nullptr) {
		from->Navigation->Right.Rule = EUINavigationRule::Stop;
		from->Navigation->Right.Widget = nullptr;
	}
	else {
		from->Navigation->Right.Rule = EUINavigationRule::Explicit;
		from->Navigation->Right.Widget = to;
	}
}

void UUITools::setNavigationVertical(UUserWidget* up, UUserWidget* down) {
	setNavigationUp(down, up);
	setNavigationDown(up, down);
}

void UUITools::setNavigationHorizontal(UUserWidget* left, UUserWidget* right) {
	setNavigationLeft(right, left);
	setNavigationRight(left, right);
}