#include "GnomePartyListWidget.h"
#include "UserManager.h"
#include "GnomeUserProfile.h"
#include "Debug.h"

void UGnomePartyListWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!UUserManager::instance || profileEntryClass == nullptr) {
		return;
	}

	UVerticalBox* profileContainer = UUITools::getVerticalBox(this, "ProfileContainer");
	if (!profileContainer) {
		return;
	}

	UUITools::syncVerticalBoxChildren(profileContainer, UUserManager::instance->loadedProfiles.Num(), profileEntryClass);
	TArray< UWidget* > 	profileEntries = profileContainer->GetAllChildren();

	int i = 0;
	for (auto& profileEntry : UUserManager::instance->loadedProfiles) {
		UUserWidget* profileWidget = Cast< UUserWidget >(profileEntries[i]);
		if (profileWidget) {

			UGnomeUserProfile* gnomeProfile = Cast< UGnomeUserProfile >(profileEntry.Value);
			if (gnomeProfile == nullptr) {
				continue;
			}

			UUITools::setTextBlockText(profileWidget, "PlayerIndex", FString::FromInt((profileEntry.Key + 1)) );
			UUITools::setTextBlockText(profileWidget, "DisplayName", gnomeProfile->username);
			UUITools::setBrushColor(profileWidget, "Background", gnomeProfile->defaultGnomeColor);
		}
		i++;
	}
}
