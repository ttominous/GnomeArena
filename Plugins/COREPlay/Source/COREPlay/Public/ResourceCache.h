#pragma once

#include "CoreMinimal.h"
#include "WidgetList.h"
#include "BodyPart.h"
#include "ItemSpec.h"
#include "PortraitStudio.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "ResourceCache.generated.h"


class UCreatureSpec;
class USceneObjectSpec;
class UAction;
class UGameConfiguration;
class UFXList;

UCLASS(Blueprintable)
class COREPLAY_API UResourceCache : public UDataAsset {

	GENERATED_BODY()

public:


	static UResourceCache* instance;

	UPROPERTY(EditAnywhere)
		TMap< FString, UTexture2D* > textures;

	UPROPERTY(EditAnywhere)
		TMap< FString, UMaterialInterface* > materials;

	UPROPERTY(EditAnywhere)
		TMap< FString, UMaterialInstanceDynamic* > materialInstances;

	UPROPERTY(EditAnywhere)
		TMap< FString, UMaterialParameterCollection* > materialParameterCollections;

	UPROPERTY()
		TMap< FString, UMaterialParameterCollectionInstance* > materialParameterCollectionInstances;

	UPROPERTY(EditAnywhere)
		TMap< FString, UStaticMesh* > staticMeshes;

	UPROPERTY(EditAnywhere)
		TMap< FString, USkeletalMesh* > skeletalMeshes;

	UPROPERTY(EditAnywhere)
		TMap< FString, USoundBase* > sounds;

	UPROPERTY(EditAnywhere)
		TMap< FString, UBodyPart* > bodyParts;

	UPROPERTY(EditAnywhere)
		TMap< FString, UItemSpec* > itemSpecs;

	UPROPERTY(EditAnywhere)
		TMap< FString, UClass* > classes;

	UPROPERTY(EditAnywhere)
		TMap< FString, TSubclassOf<UUserWidget> > widgetClasses;

	UPROPERTY(EditAnywhere)
		TMap< FString, FWidgetList > widgetLists;

	UPROPERTY(EditAnywhere)
		TMap< FString, USceneObjectSpec* > sceneObjectSpecs;

	UPROPERTY(EditAnywhere)
		TMap< FString, UCreatureSpec* > creatureSpecs;

	UPROPERTY(EditAnywhere)
		TMap< FString, UAction* > actions;

	UPROPERTY(EditAnywhere)
		TMap< FString, UGameConfiguration* > gameConfigurations;

	UPROPERTY(EditAnywhere)
		TMap< FString, UFXList* > fxLists;

	UPROPERTY(EditAnywhere)
		TMap< FString, UPortrait* > portraits;

	UPROPERTY(EditAnywhere)
		TSubclassOf< APortraitStudio > portraitStudioClass = APortraitStudio::StaticClass();

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > strings;

	static UObject* loadAsset(FString assetKey);
	static UTexture2D* getTexture(FString assetKey);
	static UMaterialInterface* getMaterial(FString assetKey);
	static UMaterialInstanceDynamic* getMaterialInstance(FString assetKey, FString sourceMaterialKey = "");
	static UMaterialParameterCollection* getMaterialParameterCollection(FString assetKey);
	static UMaterialParameterCollectionInstance* getMaterialParameterCollectionInstance(FString assetKey);
	static UStaticMesh* getStaticMesh(FString assetKey);
	static USkeletalMesh* getSkeletalMesh(FString assetKey);
	static USoundBase* getSound(FString assetKey);
	static UBodyPart* getBodyPart(FString assetKey);
	static UItemSpec* getItemSpec(FString assetKey);
	static UClass* getClass(FString assetKey);
	static UClass* getWidgetClass(FString assetKey);
	static FWidgetList getWidgetList(FString assetKey);

	static UCreatureSpec* getCreatureSpec(FString assetKey);
	static USceneObjectSpec* getSceneObjectSpec(FString assetKey);

	static UAction* getAction(FString assetKey);
	static UGameConfiguration* getGameConfiguration(FString assetKey);
	static UFXList* getFXList(FString assetKey);


	static UMaterialInstanceDynamic* getPortrait(FString key, UObject* target, bool forceRefresh);
	static bool portraitBuilt(FString key);
	static void clearPortraitsWithPrefix(FString prefix);

	static FString getString(FString assetKey);

};
