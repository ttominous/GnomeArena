#include "AssetType.h"
#include "NiagaraSystem.h"


EAssetType FAssetType::getType(UObject* object) {
	if (object != nullptr) {
		if (Cast<USkeletalMesh>(object)) {
			return EAssetType::SKELETAL_MESH;
		}
		if (Cast<UStaticMesh>(object)) {
			return EAssetType::STATIC_MESH;
		}
		if (Cast<USoundBase>(object)) {
			return EAssetType::SOUND;
		}
		if (Cast<UNiagaraSystem>(object)) {
			return EAssetType::NIAGARA_SYSTEM;
		}
	}
	return EAssetType::UNKNOWN;
}