#include "ProjectileWeaponItem.h"
#include "ProjectileWeaponItemSpec.h"

void UProjectileWeaponItem::copyTo(UItem* otherItem) {
	Super::copyTo(otherItem);

	UProjectileWeaponItem* otherProjectileWeaponItem = Cast< UProjectileWeaponItem >(otherItem);
	if (otherProjectileWeaponItem) {
		otherProjectileWeaponItem->ammoItemKey = ammoItemKey;
		otherProjectileWeaponItem->clipCount = clipCount;
	}
}

void UProjectileWeaponItem::initNew() {
	Super::initNew();

	UProjectileWeaponItemSpec* projectileItemSpec = Cast< UProjectileWeaponItemSpec >(getItemSpec());
	if (!projectileItemSpec) {
		return;
	}
	ammoItemKey = projectileItemSpec->defaultAmmoItemSpecKey;
	clipCount = projectileItemSpec->defaultClipCount;
}
