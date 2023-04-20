#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
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
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/BorderSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/UniformGridSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/Spacer.h"
#include "Components/CheckBox.h"

#include "Blueprint/WidgetTree.h"

#include "UITools.generated.h"


UCLASS()
class COREPLAY_API UUITools : public UObject {

	GENERATED_BODY()

public:

	static UWidget* getWidget(UUserWidget* parent, FString name);
	static UTextBlock* getTextBlock(UUserWidget* parent, FString name);
	static UScrollBox* getScrollBox(UUserWidget* parent, FString name);
	static UImage* getImage(UUserWidget* parent, FString name);
	static UBorder* getBorder(UUserWidget* parent, FString name);
	static UCanvasPanel* getCanvasPanel(UUserWidget* parent, FString name);
	static UProgressBar* getProgressBar(UUserWidget* parent, FString name);
	static UVerticalBox* getVerticalBox(UUserWidget* parent, FString name);
	static UHorizontalBox* getHorizontalBox(UUserWidget* parent, FString name);

	UFUNCTION(BlueprintCallable)
		static UButton* getButton(UUserWidget* parent, FString name);

	static UListView* getListView(UUserWidget* parent, FString name);
	static USlider* getSlider(UUserWidget* parent, FString name);
	static UCheckBox* getCheckBox(UUserWidget* parent, FString name);
	static UOverlay* getOverlay(UUserWidget* parent, FString name);
	static UComboBoxString* getComboBox(UUserWidget* parent, FString name);

	UFUNCTION(BlueprintCallable)
		static UEditableTextBox* getEditableTextBox(UUserWidget* parent, FString name);

	UFUNCTION(BlueprintCallable)
		static UEditableText* getEditableText(UUserWidget* parent, FString name);


	static UUniformGridPanel* getUniformGridPanel(UUserWidget* parent, FString name);
	static USpacer* getSpacer(UUserWidget* parent, FString name);
	static UUserWidget* getUserWidget(UUserWidget* parent, FString name);

	UFUNCTION(BlueprintCallable)
		static void setBrushColor(UUserWidget* parent, FString name, FSlateColor color);

	static void setBrushColor(UWidget* widget, FSlateColor color);

	UFUNCTION(BlueprintCallable)
		static void setTextBlockColor(UUserWidget* parent, FString name, FSlateColor color);
	static void setTextBlockColor(UTextBlock* widget, FSlateColor color);

	UFUNCTION(BlueprintCallable)
		static void setTextBlockText(UUserWidget* parent, FString name, FString text);
	static void setTextBlockText(UTextBlock* widget, FString text);

	static void setTextBlockFontSize(UUserWidget* parent, FString name, int fontSize);
	UFUNCTION(BlueprintCallable)
		static void setTextBlockFontSize(UTextBlock* widget, int fontSize);

	static void setButtonEnabled(UUserWidget* parent, FString name, bool enabled);
	static void setButtonEnabled(UButton* widget, bool enabled);

	UFUNCTION(BlueprintCallable)
		static void setImageBrushTexture(UUserWidget* parent, FString name, UTexture2D* texture);

	static void setImageBrushTexture(UImage* widget, UTexture2D* texture);

	UFUNCTION(BlueprintCallable)
		static void setImageBrushMaterial(UUserWidget* parent, FString name, UMaterialInstance* material);

	static void setImageBrushMaterial(UImage* widget, UMaterialInstance* material);

	static void setProgressBarPerc(UUserWidget* parent, FString name, float perc);
	static void setProgressBarPerc(UProgressBar* widget, float perc);

	static void setSliderBounds(UUserWidget* parent, FString name, float current, float min, float max, float step);
	static void setSliderBounds(USlider* widget, float current, float min, float max, float step);

	static void setSliderValue(UUserWidget* parent, FString name, float value);
	static void setSliderValue(USlider* widget, float value);

	static void setComboBoxValues(UUserWidget* parent, FString name, TArray< FString > values, FString currentValue);
	static void setComboBoxValues(UComboBoxString* widget, TArray< FString > values, FString currentValue);

	UFUNCTION(BlueprintCallable)
		static void setEditableTextBoxValue(UUserWidget* parent, FString name, FString value);
	static void setEditableTextBoxValue(UEditableTextBox* widget, FString value);

	UFUNCTION(BlueprintCallable)
		static void setEditableTextValue(UUserWidget* parent, FString name, FString value);
	static void setEditableTextValue(UEditableText* widget, FString value);

	static void setVisibility(UUserWidget* parent, FString name, ESlateVisibility visibility);
	static void setVisibility(UWidget* widget, ESlateVisibility visibility);

	static float getSliderValue(UUserWidget* parent, FString name);
	static float getSliderValue(USlider* widget);

	static FString getComboBoxValue(UUserWidget* parent, FString name);
	static FString getComboBoxValue(UComboBoxString* widget);

	UFUNCTION(BlueprintCallable)
		static FString getEditableTextBoxValue(UUserWidget* parent, FString name);
	static FString getEditableTextBoxValue(UEditableTextBox* widget);

	UFUNCTION(BlueprintCallable)
		static FString getEditableTextValue(UUserWidget* parent, FString name);
	static FString getEditableTextValue(UEditableText* widget);

	static UUserWidget* spawnWidget(UClass* widgetClass, UWidget* parent);
	static UUserWidget* spawnWidget(FString name, UClass* widgetClass, UWidget* parent);
	static UUserWidget* spawnWidgetToGrid(UClass* widgetClass, int x, int y, UUniformGridPanel* grid);

	static void setToolTipText(UUserWidget* parent, FString name, FString text);
	static void setToolTipText(UWidget* widget, FString text);
	static UUserWidget* setToolTipWidgetClass(UUserWidget* parent, FString name, UClass* widgetClass);
	static UUserWidget* setToolTipWidgetClass(UWidget* widget, UClass* widgetClass);

	static FVector2D getAbsoluteSize(UWidget* widget);
	static FVector2D getLocalSize(UWidget* widget);
	static FVector2D getPosition(UWidget* widget, bool inViewportSpace);
	static FVector2D convertToViewportPos(UWidget* widget, FVector2D pos);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static UUserWidget* getParentUserWidget(UWidget* widget);

	UFUNCTION(BlueprintCallable)
		static bool syncVerticalBoxChildren(UVerticalBox* verticalBox, int numberOfChildren, UClass* childWidgetClass);

	UFUNCTION(BlueprintCallable)
		static bool syncHorizontalBoxChildren(UHorizontalBox* horizontalBox, int numberOfChildren, UClass* childWidgetClass);

	UFUNCTION(BlueprintCallable)
		static bool syncUniformGridChildren(UUniformGridPanel* grid, int numberOfChildren, int width, UClass* childWidgetClass);

	static void setPosition(UWidget* widget, FVector2D position);
	static void setPosition(UPanelSlot* slot, FVector2D position);

	static void setSize(UWidget* widget, FVector2D size);
	static void setSize(UPanelSlot* slot, FVector2D size);

	static TArray< UWidget* > getAllChildren(UUserWidget* root);
	static void getAllChildren(UUserWidget * root, TArray< UWidget* >&collection, UWidget * parent);
	static bool isChildOf(UUserWidget * root, UUserWidget * target);
	static FString getPath(UUserWidget* target);

	static void setNavigationUp(UUserWidget* from, UUserWidget* to);
	static void setNavigationDown(UUserWidget* from, UUserWidget* to);
	static void setNavigationLeft(UUserWidget* from, UUserWidget* to);
	static void setNavigationRight(UUserWidget* from, UUserWidget* to);
	static void setNavigationVertical(UUserWidget* up, UUserWidget* down);
	static void setNavigationHorizontal(UUserWidget* left, UUserWidget* right);


};	

