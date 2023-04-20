#include "BodyPart.h"
#include "BodyComponent.h"
#include "StringTools.h"
#include "ResourceCache.h"
#include "SettingsTools.h"
#include "Debug.h"

#include "Materials/MaterialInstanceConstant.h"


FBodyPartDetail FBodyPartDetail::EMPTY_DETAILS;


bool FBodyPartMaterial::instanced() {
	return relevantParameters.Num() > 0;
}

UMaterialInstance* FBodyPartMaterial::getMaterialInstance(UMeshComponent* meshComponent) {
	if (sharedMaterial == nullptr) {
		slotIndex = meshComponent->GetMaterialSlotNames().Find(FName(*slot));
		if (slotIndex < 0) {
			UDebug::error("Could not find " + slot);
			return nullptr;
		}

		UMaterialInterface* baseMaterial = materialOverride ? materialOverride : meshComponent->GetMaterial(slotIndex);

		UMaterialInstanceDynamic* dynamicMaterial = nullptr;
		if (lodPicker != nullptr) {
			UMaterialInterface* lodMaterial = lodPicker->getMaterial(USettingsTools::materialSettings);
			if (lodMaterial) {
				dynamicMaterial = UMaterialInstanceDynamic::Create(lodMaterial, UResourceCache::instance);
				if (baseMaterial) {
					UMaterialInstance* baseMaterialInstance = Cast< UMaterialInstance >(baseMaterial);
					dynamicMaterial->CopyMaterialUniformParameters(baseMaterial);
					if (baseMaterialInstance) {
						dynamicMaterial->CopyParameterOverrides(baseMaterialInstance);
						dynamicMaterial->CopyInterpParameters(baseMaterialInstance);
					}
				}
			}
		}
		if (!dynamicMaterial) {
			if (!baseMaterial) {
				return nullptr;
			}
			dynamicMaterial = UMaterialInstanceDynamic::Create(baseMaterial, UResourceCache::instance);
		}
		if (!dynamicMaterial) {
			return nullptr;
		}

		for (auto& parameter : scalarParameters) {
			FString finalKey = parameterForwarding.Contains(parameter.Key) ? parameterForwarding[parameter.Key] : parameter.Key;
			dynamicMaterial->SetScalarParameterValue(FName(*finalKey), parameter.Value);
		}
		for (auto& parameter : vectorParameters) {
			FString finalKey = parameterForwarding.Contains(parameter.Key) ? parameterForwarding[parameter.Key] : parameter.Key;
			dynamicMaterial->SetVectorParameterValue(FName(*finalKey), parameter.Value);
		}
		for (auto& parameter : textureParameters) {
			FString finalKey = parameterForwarding.Contains(parameter.Key) ? parameterForwarding[parameter.Key] : parameter.Key;
			dynamicMaterial->SetTextureParameterValue(FName(*finalKey), UResourceCache::getTexture(parameter.Value));
		}
		if (instanced()) {
			return dynamicMaterial;
		}
		sharedMaterial = dynamicMaterial;
	}

	return sharedMaterial;
}



EAssetType FBodyPartDetail::getType() {
	return FAssetType::getType(asset);
}

UStaticMesh* FBodyPartDetail::getStaticMesh() {
	return Cast< UStaticMesh >(asset);
}

USkeletalMesh* FBodyPartDetail::getSkeletalMesh() {
	return Cast< USkeletalMesh >(asset);
}


FBodyPartDetail& UBodyPart::getDetails(FString slotKey, UBodyComponent* component) {
	return FBodyPartDetail::EMPTY_DETAILS;
}

FBodyPartDetail& USingleBodyPart::getDetails(FString slotKey, UBodyComponent* component) {
	return defaultDetails;
}

FString UConditionalBodyPart::getKey(FString slotKey, UBodyComponent* component) {
	return "";
}

FBodyPartDetail& UConditionalBodyPart::getDetails(FString slotKey, UBodyComponent* component) {
	FString key = getKey(slotKey, component);
	if (variations.Contains(key)) {
		return variations[key];
	}
	return defaultDetails;
}