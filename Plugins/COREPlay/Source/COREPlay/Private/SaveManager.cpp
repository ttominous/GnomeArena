#include "SaveManager.h"
#include "ComplexGameState.h"
#include "Creature.h"
#include "CreatureSpec.h"
#include "SceneObject.h"
#include "SceneObjectSpec.h"
#include "DropItem.h"
#include "ResourceCache.h"
#include "TimeTools.h"
#include "FileTools.h"
#include "Debug.h"

USaveManager* USaveManager::instance = nullptr;


void USaveManager::init() {

}

void USaveManager::executeSave() {

	FXMLBuilder saveBuilder;

	if (saveAllCreatures) {
		if (ACreature::instances.Num() > 0) {
			FXMLBuilderNode& creatures = saveBuilder.addNode("Creatures");
			for (ACreature* creature : ACreature::instances) {
				if (creature == nullptr) {
					continue;
				}
				FXMLBuilderNode& creatureNode = creatures.addChild("Creature");
				writeCreature(creature, creatureNode);
			}
		}
	}

	if (saveLastCreatureID) {
		saveBuilder.root.setAttribute("lastCreatureID", (double)ACreature::lastID);
	}

	if (saveAllSceneObjects) {
		if (ASceneObject::instances.Num() > 0) {
			FXMLBuilderNode& sceneObjects = saveBuilder.addNode("SceneObjects");
			for (ASceneObject* sceneObject : ASceneObject::instances) {
				if (sceneObject == nullptr || sceneObject->isGhost) {
					continue;
				}
				FXMLBuilderNode& sceneObjectNode = sceneObjects.addChild("SceneObject");
				writeSceneObject(sceneObject, sceneObjectNode);
			}
		}
	}

	if (saveLastSceneObjectID) {
		saveBuilder.root.setAttribute("lastSceneObjectID", (double)ASceneObject::lastID);
	}


	if (saveLastDropItemID) {
		saveBuilder.root.setAttribute("lastDropItemID", (double)ADropItem::lastID);
	}

	if (saveAllDropItems) {
		if (ASceneObject::instances.Num() > 0) {
			FXMLBuilderNode& dropItems = saveBuilder.addNode("DropItems");
			for (ADropItem* dropItem : ADropItem::instances) {
				if (dropItem == nullptr) {
					continue;
				}
				FXMLBuilderNode& dropItemNode = dropItems.addChild("DropItem");
				writeDropItem(dropItem, dropItemNode);
			}
		}
	}

	writeOthers(saveBuilder.root);

	UFileTools::saveFile(saveFolderPath + saveFileName, saveBuilder.toString());
}

void USaveManager::writeCreature(ACreature* creature, FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("id", (double)creature->id);
	rootNode.setAttribute("name", creature->name);
	if (creature->spec) {
		rootNode.setAttribute("spec", creature->spec->key);
	}
	else {
		rootNode.setAttribute("class", creature->GetClass()->GetPathName());
	}
	rootNode.setAttribute("location", creature->GetActorLocation());
	rootNode.setAttribute("yaw", creature->bodyYaw);
	rootNode.setAttribute("dead", creature->dead);
	creature->writeXML(rootNode);
}

void USaveManager::writeSceneObject(ASceneObject* sceneObject, FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("id", (double)sceneObject->id);
	if (sceneObject->spec) {
		rootNode.setAttribute("spec", sceneObject->spec->key);
	}
	else {
		rootNode.setAttribute("class", sceneObject->GetClass()->GetPathName());
	}
	rootNode.setAttribute("location", sceneObject->GetActorLocation());
	rootNode.setAttribute("yaw", sceneObject->GetActorRotation().Yaw);
	sceneObject->writeXML(rootNode);
}

void USaveManager::writeDropItem(ADropItem* dropItem, FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("id", (double)dropItem->id);
	rootNode.setAttribute("class", dropItem->GetClass()->GetPathName());
	rootNode.setAttribute("location", dropItem->GetActorLocation());
	rootNode.setAttribute("yaw", dropItem->GetActorRotation().Yaw);
	dropItem->writeXML(rootNode);
}

void USaveManager::writeOthers(FXMLBuilderNode& rootNode) {

}

void USaveManager::executeLoad() {

	//UDebug::print("[" + UTimeTools::getSytemDateTimeString() + "] Started load", FColor::Cyan, 60.0f);

	deferredLoadCreatures.Empty();
	deferredLoadSceneObjects.Empty();

	FXmlNode* rootNode = UXMLTools::getXMLRoot(saveFolderPath + saveFileName, false);
	if (rootNode == nullptr) {
		return;
	}

	if (UXMLTools::hasAttribute(rootNode, "lastCreatureID")) {
		ACreature::lastID = UXMLTools::getAttributeInt(rootNode, "lastCreatureID", 0);
		UResourceCache::clearPortraitsWithPrefix("creature.");
	}
	if (UXMLTools::hasAttribute(rootNode, "lastSceneObjectID")) {
		ASceneObject::lastID = UXMLTools::getAttributeInt(rootNode, "lastSceneObjectID", 0);
	}
	if (UXMLTools::hasAttribute(rootNode, "lastDropItemID")) {
		ADropItem::lastID = UXMLTools::getAttributeInt(rootNode, "lastDropItemID", 0);
	}

	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();

	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("CREATURES")) {
			TArray< FXmlNode* > creatureNodes = childNode->GetChildrenNodes();
			for (FXmlNode* creatureNode : creatureNodes) {
				readCreature(creatureNode);
			}
		}
		else if (tag.Equals("SCENEOBJECTS")) {
			TArray< FXmlNode* > sceneObjectNodes = childNode->GetChildrenNodes();
			for (FXmlNode* sceneObjectNode : sceneObjectNodes) {
				readSceneObject(sceneObjectNode);
			}
		}
		else if (tag.Equals("DROPITEMS")) {
			TArray< FXmlNode* > dropItemNodes = childNode->GetChildrenNodes();
			for (FXmlNode* dropItemNode : dropItemNodes) {
				readDropItem(dropItemNode);
			}
		}
	}
	readOthers(rootNode);

	for (auto& creature : deferredLoadCreatures) {
		creature.Key->readXML(creature.Value);
	}
	deferredLoadCreatures.Empty();

	for (auto& sceneObject : deferredLoadSceneObjects) {
		sceneObject.Key->readXML(sceneObject.Value);
	}
	deferredLoadSceneObjects.Empty();

	for (auto& dropItem : deferredLoadDropItems) {
		dropItem.Key->readXML(dropItem.Value);
	}
	deferredLoadDropItems.Empty();

	//UDebug::print("[" + UTimeTools::getSytemDateTimeString() + "] Finished load", FColor::Cyan, 60.0f);

}

ACreature* USaveManager::readCreature(FXmlNode* rootNode) {
	FString specKey = UXMLTools::getAttributeString(rootNode, "spec");
	FString classKey = UXMLTools::getAttributeString(rootNode, "class");
	FVector location = UXMLTools::getAttributeVector(rootNode, "location");
	float yaw = UXMLTools::getAttributeFloat(rootNode, "yaw");

	ACreature* creature = nullptr;
	if (!specKey.Equals("")) {
		UCreatureSpec* spec = UResourceCache::getCreatureSpec(specKey);
		creature = ACreature::spawn(GetWorld(), spec, location, yaw);
	}
	else if (!classKey.Equals("")) {
		UClass* spawnClass = UResourceCache::getClass(classKey);
		creature = ACreature::spawn(GetWorld(), spawnClass, location, yaw);
	}
	else {
		return nullptr;
	}


	if (creature) {
		creature->id = UXMLTools::getAttributeInt(rootNode, "id", creature->id);
		creature->name = UXMLTools::getAttributeString(rootNode, "name", creature->name);
		creature->dead = UXMLTools::getAttributeBool(rootNode, "dead", creature->dead);
		deferredLoadCreatures.Add(creature, rootNode);
	}
	return creature;
}

ASceneObject* USaveManager::readSceneObject(FXmlNode* rootNode) {
	FString specKey = UXMLTools::getAttributeString(rootNode, "spec");
	FString classKey = UXMLTools::getAttributeString(rootNode, "class");
	FVector location = UXMLTools::getAttributeVector(rootNode, "location");
	float yaw = UXMLTools::getAttributeFloat(rootNode, "yaw");

	ASceneObject* sceneObject = nullptr;
	if (!specKey.Equals("")) {
		USceneObjectSpec* spec = UResourceCache::getSceneObjectSpec(specKey);
		sceneObject = ASceneObject::spawn(GetWorld(), spec, location, yaw, false);
	}
	else if (!classKey.Equals("")) {
		UClass* spawnClass = UResourceCache::getClass(classKey);
		sceneObject = ASceneObject::spawn(GetWorld(), spawnClass, location, yaw, false);
	}
	else {
		return nullptr;
	}

	if (sceneObject) {
		sceneObject->id = UXMLTools::getAttributeInt(rootNode, "id", sceneObject->id);
		sceneObject->readXML(rootNode);
		deferredLoadSceneObjects.Add(sceneObject, rootNode);
	}
	return sceneObject;
}


ADropItem* USaveManager::readDropItem(FXmlNode* rootNode) {
	FString classKey = UXMLTools::getAttributeString(rootNode, "class");
	FVector location = UXMLTools::getAttributeVector(rootNode, "location");
	float yaw = UXMLTools::getAttributeFloat(rootNode, "yaw");

	ADropItem* dropItem = nullptr;
	if (!classKey.Equals("")) {
		UClass* spawnClass = UResourceCache::getClass(classKey);
		dropItem = ADropItem::spawn(GetWorld(), spawnClass, location, yaw, false);
	}
	else {
		return nullptr;
	}

	if (dropItem) {
		dropItem->id = UXMLTools::getAttributeInt(rootNode, "id", dropItem->id);
		dropItem->readXML(rootNode);
		deferredLoadDropItems.Add(dropItem, rootNode);
	}
	return dropItem;
}

void USaveManager::readOthers(FXmlNode* rootNode) {

}


void USaveManager::save() {
	if (instance != nullptr) {
		instance->executeSave();
	}
}

void USaveManager::load() {
	if (instance != nullptr) {

		AComplexGameState* gameState = Cast< AComplexGameState >(instance->GetWorld()->GetGameState());

		if (instance->tearDownOnLoad) {
			if (gameState) {
				gameState->tearDown();
			}
		}

		instance->executeLoad();
		if (gameState) {
			gameState->handleLoadComplete();
		}
	}
}