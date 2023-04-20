#include "Item.h"
#include "ItemSpec.h"
#include "ResourceCache.h"



UItemSpec* UItem::getItemSpec() {
	if (itemSpec == nullptr) {
		itemSpec = UResourceCache::getItemSpec(itemSpecKey);
	}
	return itemSpec;
}

void UItem::copyTo(UItem* otherItem) {
	otherItem->itemSpecKey = itemSpecKey;
	otherItem->itemSpec = itemSpec;
	otherItem->quantity = quantity;
	otherItem->durability = durability;
}

void UItem::initNew() {

}