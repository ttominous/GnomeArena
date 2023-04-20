#include "DamageDetails.h"

FDamageDetails FDamageDetails::add(FDamageDetails& other) {
	FDamageDetails result;

	result.damages = damages;
	for (int d = 0; d < other.damages.Num(); d++) {

		bool unique = true;
		for (int d2 = 0; d2 < result.damages.Num(); d2++) {
			if (result.damages[d2].type == other.damages[d].type) {
				result.damages[d2].amount += other.damages[d].amount;
				if (other.damages[d].critical) {
					result.damages[d2].critical = true;
				}
				unique = false;
				break;
			}
		}

		if (unique) {
			result.damages.Add(other.damages[d]);
		}
	}


	result.attacker = attacker;
	if (other.attacker.isPopulated()) {
		result.attacker = other.attacker;
	}

	result.victim = victim;
	if (other.victim.isPopulated()) {
		result.victim = other.victim;
	}

	result.description = description;
	if (other.description.Len() > 0) {
		result.description = other.description;
	}

	result.iconKey = iconKey;
	if (other.iconKey.Len() > 0) {
		result.iconKey = other.iconKey;
	}

	result.effectKey = effectKey;
	if (other.effectKey.Len() > 0) {
		result.effectKey = other.effectKey;
	}

	result.force = force + other.force;
	result.stagger = stagger + other.stagger;

	result.forceFaceAttackerAim = other.forceFaceAttackerAim;
	result.forceOnlyUsesXY = other.forceOnlyUsesXY;

	return result;
}