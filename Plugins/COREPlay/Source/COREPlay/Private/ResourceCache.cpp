#include "ResourceCache.h"

#include "Blueprint/UserWidget.h"
#include "Action.h"
#include "GameConfiguration.h"
#include "FXList.h"

#include "Debug.h"

#include "Kismet/GameplayStatics.h"



UResourceCache* UResourceCache::instance = nullptr;


UObject* UResourceCache::loadAsset(FString assetKey) {
	if (assetKey.Len() == 0 || !assetKey.StartsWith("/")) {
		return nullptr;
	}
	FSoftObjectPath assetPath(assetKey);
	return assetPath.TryLoad();
}

UTexture2D* UResourceCache::getTexture(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->textures.Contains(assetKey)) {
		instance->textures.Add(assetKey, Cast< UTexture2D >(loadAsset(assetKey)));
	}
	return instance->textures[assetKey];
}


UMaterialInterface* UResourceCache::getMaterial(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->materials.Contains(assetKey)) {
		instance->materials.Add(assetKey, Cast< UMaterialInterface >(loadAsset(assetKey)));
	}
	return instance->materials[assetKey];
}

UMaterialInstanceDynamic* UResourceCache::getMaterialInstance(FString assetKey, FString sourceMaterialKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->materialInstances.Contains(assetKey)) {

		if (sourceMaterialKey.Len() > 0) {
			UMaterialInterface* material = getMaterial(sourceMaterialKey);
			if (material) {
				UMaterialInstanceDynamic* newMaterial = UMaterialInstanceDynamic::Create(material, instance);
				instance->materialInstances.Add(assetKey, newMaterial);
				return newMaterial;
			}
		}

		return nullptr;
	}
	return instance->materialInstances[assetKey];
}

UMaterialParameterCollection* UResourceCache::getMaterialParameterCollection(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->materialParameterCollections.Contains(assetKey)) {
		instance->materialParameterCollections.Add(assetKey, Cast< UMaterialParameterCollection >(loadAsset(assetKey)));
	}
	return instance->materialParameterCollections[assetKey];
}

UMaterialParameterCollectionInstance* UResourceCache::getMaterialParameterCollectionInstance(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->materialParameterCollectionInstances.Contains(assetKey)) {
		UMaterialParameterCollection* collection = getMaterialParameterCollection(assetKey);
		UMaterialParameterCollectionInstance* collectionInstance = collection != nullptr ? instance->GetWorld()->GetParameterCollectionInstance(collection) : nullptr;
		instance->materialParameterCollectionInstances.Add(assetKey, collectionInstance);
	}
	return instance->materialParameterCollectionInstances[assetKey];
}


UStaticMesh* UResourceCache::getStaticMesh(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->staticMeshes.Contains(assetKey)) {
		instance->staticMeshes.Add(assetKey, Cast< UStaticMesh >(loadAsset(assetKey)));
	}
	return instance->staticMeshes[assetKey];
}


USkeletalMesh* UResourceCache::getSkeletalMesh(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->skeletalMeshes.Contains(assetKey)) {
		instance->skeletalMeshes.Add(assetKey, Cast< USkeletalMesh >(loadAsset(assetKey)));
	}
	return instance->skeletalMeshes[assetKey];
}


USoundBase* UResourceCache::getSound(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->sounds.Contains(assetKey)) {
		instance->sounds.Add(assetKey, Cast< USoundBase >(loadAsset(assetKey)));
	}
	return instance->sounds[assetKey];
}

UBodyPart* UResourceCache::getBodyPart(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->bodyParts.Contains(assetKey)) {
		instance->bodyParts.Add(assetKey, Cast< UBodyPart >(loadAsset(assetKey)));
	}
	return instance->bodyParts[assetKey];
}

UItemSpec* UResourceCache::getItemSpec(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->itemSpecs.Contains(assetKey)) {
		instance->itemSpecs.Add(assetKey, Cast< UItemSpec >(loadAsset(assetKey)));
	}
	return instance->itemSpecs[assetKey];
}

UClass* UResourceCache::getClass(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->classes.Contains(assetKey)) {

		UClass* loadedClass = nullptr;
		if (assetKey.EndsWith("_C") || assetKey.Contains("/Script/")) {
			loadedClass = LoadClass< UObject >(instance, *assetKey, NULL, LOAD_None, NULL);
		}
		else {
			UBlueprint* blueprint = Cast< UBlueprint >(loadAsset(assetKey));
			if (blueprint) {
				loadedClass = blueprint->GeneratedClass;
			}
		}
		instance->classes.Add(assetKey, loadedClass);
	}
	return instance->classes[assetKey];
}

UClass* UResourceCache::getWidgetClass(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->widgetClasses.Contains(assetKey)) {

		UClass* loadedClass = nullptr;
		if (assetKey.EndsWith("_C") || assetKey.Contains("/Script/")) {
			loadedClass = LoadClass< UObject >(instance, *assetKey, NULL, LOAD_None, NULL);
		}
		else {
			UBlueprint* blueprint = Cast< UBlueprint >(loadAsset(assetKey));
			if (blueprint) {
				loadedClass = blueprint->GeneratedClass;
			}
		}
		instance->widgetClasses.Add(assetKey, loadedClass);
	}
	return instance->widgetClasses[assetKey];
}

FWidgetList UResourceCache::getWidgetList(FString assetKey) {
	if (!instance) {
		return FWidgetList();
	}
	if (!instance->widgetLists.Contains(assetKey)) {
		instance->widgetLists.Add(assetKey, FWidgetList());
	}
	return instance->widgetLists[assetKey];
}

UCreatureSpec* UResourceCache::getCreatureSpec(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	return instance->creatureSpecs.Contains(assetKey) ? instance->creatureSpecs[assetKey] : nullptr;
}

USceneObjectSpec* UResourceCache::getSceneObjectSpec(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	return instance->sceneObjectSpecs.Contains(assetKey) ? instance->sceneObjectSpecs[assetKey] : nullptr;
}

UAction* UResourceCache::getAction(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->actions.Contains(assetKey)) {
		instance->actions.Add(assetKey, Cast< UAction >(loadAsset(assetKey)));
	}
	return instance->actions[assetKey];
}

UGameConfiguration* UResourceCache::getGameConfiguration(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->gameConfigurations.Contains(assetKey)) {
		instance->gameConfigurations.Add(assetKey, Cast< UGameConfiguration >(loadAsset(assetKey)));
	}
	return instance->gameConfigurations[assetKey];
}

UFXList* UResourceCache::getFXList(FString assetKey) {
	if (!instance) {
		return nullptr;
	}
	if (!instance->fxLists.Contains(assetKey)) {
		instance->fxLists.Add(assetKey, Cast< UFXList >(loadAsset(assetKey)));
	}
	return instance->fxLists[assetKey];
}


UMaterialInstanceDynamic* UResourceCache::getPortrait(FString key, UObject* target, bool forceRefresh) {
	if (!instance) {
		return nullptr;
	}

	UMaterialInstanceDynamic* previous = nullptr;
	if (instance->portraits.Contains(key) && !forceRefresh) {
		return instance->portraits[key]->material;
	}

	APortraitStudio* studio = nullptr;
	TArray<AActor*> portraitActors;
	UGameplayStatics::GetAllActorsOfClass(instance->GetWorld(), APortraitStudio::StaticClass(), portraitActors);
	if (portraitActors.Num() == 0) {
		FActorSpawnParameters params;
		params.bNoFail = true;
		FVector location(0, 0, -10000);
		studio = instance->GetWorld()->SpawnActor<APortraitStudio>(instance->portraitStudioClass, location, FRotator::ZeroRotator, params);
		studio->init();
		studio->SetActorLocation(location);
		studio->transform.SetLocation(location);
	}
	else {
		studio = Cast< APortraitStudio >(portraitActors[0]);
	}

	UPortrait* portrait = nullptr;
	if (instance->portraits.Contains(key)) {
		portrait = instance->portraits[key];
	}
	else {
		portrait = NewObject< UPortrait >(instance);
		portrait->key = key;
		UMaterialInterface* baseMaterial = getMaterial("PortraitDefault");
		portrait->material = UMaterialInstanceDynamic::Create(baseMaterial, portrait);
		portrait->target = target;
		instance->portraits.Add(key, portrait);
	}
	portrait->waitTime = -1.0f;
	studio->addOrder(portrait);
	return instance->portraits[key]->material;
}

bool UResourceCache::portraitBuilt(FString key) {
	return instance && instance->portraits.Contains(key) && instance->portraits[key]->built;
}

void UResourceCache::clearPortraitsWithPrefix(FString prefix) {
	if (instance) {
		TArray< FString > portraitsToRemove;
		for (auto portrait : instance->portraits) {
			if (portrait.Key.StartsWith(prefix)) {
				portraitsToRemove.Add(portrait.Key);
			}
		}
		for (FString portraitKey : portraitsToRemove) {
			instance->portraits.Remove(portraitKey);
		}
	}
}

FString UResourceCache::getString(FString assetKey) {
	if (instance) {
		return instance->strings.Contains(assetKey) ? instance->strings[assetKey] : "";
	}
	return "";
}