#pragma once

#include "CoreMinimal.h"
#include "XMLTools.h"

#include "SaveManager.generated.h"

class ACreature;
class ASceneObject;
class ADropItem;

UCLASS()
class COREPLAY_API USaveManager : public UObject {

	GENERATED_BODY()

public:

	static USaveManager* instance;

	UPROPERTY(EditAnywhere)
		FString saveFolderPath;

	UPROPERTY(EditAnywhere)
		FString saveFileName;

	UPROPERTY(EditAnywhere)
		bool saveAllCreatures = true;

	UPROPERTY(EditAnywhere)
		bool saveLastCreatureID = true;

	UPROPERTY(EditAnywhere)
		bool saveLastSceneObjectID = true;

	UPROPERTY(EditAnywhere)
		bool saveAllSceneObjects = true;

	UPROPERTY(EditAnywhere)
		bool saveLastDropItemID = true;

	UPROPERTY(EditAnywhere)
		bool saveAllDropItems = true;

	UPROPERTY(EditAnywhere)
		bool tearDownOnLoad = true;
		
	TMap< ACreature*, FXmlNode* > deferredLoadCreatures;
	TMap< ASceneObject*, FXmlNode* > deferredLoadSceneObjects;
	TMap< ADropItem*, FXmlNode* > deferredLoadDropItems;

	virtual void init();
	virtual void executeSave();
	virtual void writeCreature(ACreature* creature, FXMLBuilderNode& rootNode);
	virtual void writeSceneObject(ASceneObject* sceneObject, FXMLBuilderNode& rootNode);
	virtual void writeDropItem(ADropItem* dropItem, FXMLBuilderNode& rootNode);
	virtual void writeOthers(FXMLBuilderNode& rootNode);

	virtual void executeLoad();
	virtual ACreature* readCreature(FXmlNode* rootNode);
	virtual ASceneObject* readSceneObject(FXmlNode* rootNode);
	virtual ADropItem* readDropItem(FXmlNode* rootNode);
	virtual void readOthers(FXmlNode* rootNode);

	UFUNCTION(BlueprintCallable)
		static void save();

	UFUNCTION(BlueprintCallable)
		static void load();

};