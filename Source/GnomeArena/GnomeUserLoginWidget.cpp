#include "GnomeUserLoginWidget.h"
#include "GnomeUserProfile.h"
#include "Debug.h"

void UGnomeUserLoginWidget::applyCreateProfileOtherInputs(UUserProfile* profile) {

	UGnomeUserProfile* gnomeProfile = Cast< UGnomeUserProfile >(profile);
	if (!gnomeProfile) {
		return;
	}

	UColorPicker* colorInput = Cast< UColorPicker >(UUITools::getWidget(this, "CreateNew_Color"));
	if (!colorInput) {
		UDebug::error("Missing CreateNew_Color field.");
		return;
	}
	gnomeProfile->defaultGnomeColor = colorInput->currentColor;

}

void UGnomeUserLoginWidget::applyCreateProfileTransactionExtraData(UUserLoginTransaction* transaction, UUserProfile* profile) {
	UGnomeUserProfile* gnomeProfile = Cast< UGnomeUserProfile >(profile);
	if (!gnomeProfile) {
		return;
	}
	UJsonTools::setColor(transaction->extraData, "defaultGnomeColor", gnomeProfile->defaultGnomeColor);
}