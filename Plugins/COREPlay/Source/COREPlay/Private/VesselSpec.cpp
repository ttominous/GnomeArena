#include "VesselSpec.h"
#include "Debug.h"

int UVesselSpec::getSlotIndex(FString slotKey) {
	if (slotMap.Contains(slotKey)) {
		return slotMap[slotKey];
	}
	int i = 0;
	for (auto entry : slots) {
		if (entry.Key.Equals(slotKey)) {
			slotMap.Add(slotKey, i);
			return i;
		}
		i++;
	}
	return -1;
}