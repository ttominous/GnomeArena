#include "StatSet.h"

bool FStatSet::hasStat(FString key) {
	key = key.ToUpper();
	if (statsCache.Contains(key)) {
		return true;
	}
	for (int i = 0; i < stats.Num(); i++) {
		if (stats[i].key.ToUpper().Equals(key)) {
			statsCache.Add(key, i);
			return true;
		}
	}
	return false;
}

FStat& FStatSet::get(FString key) {
	key = key.ToUpper();
	if (!statsCache.Contains(key)) {
		for (int i = 0; i < stats.Num(); i++) {
			if (stats[i].key.ToUpper().Equals(key)) {
				statsCache.Add(key, i);
				return stats[i];
			}
		}
	}
	return stats[statsCache[key]];
}