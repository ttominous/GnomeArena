#include "MaterialPicker.h"
#include "Debug.h"

UMaterialInterface* UMaterialPicker::getMaterial(TMap<FString, int>& materialSettings) {
	for (FMaterialPickerOption& optionalMaterial : optionalMaterials) {
		bool valid = true;
		for (auto condition : optionalMaterial.conditions) {
			if (!materialSettings.Contains(condition.Key) || materialSettings[condition.Key] < condition.Value) {
				valid = false;
				break;
			}
		}
		if (valid) {
			return optionalMaterial.material;
		}
	}
	return defaultMaterial;
}