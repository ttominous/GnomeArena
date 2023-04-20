#include "GnomeUserManager.h"
#include "GnomeUserProfile.h"

UClass* UGnomeUserManager::getProfileClass() {
	return UGnomeUserProfile::StaticClass();
}