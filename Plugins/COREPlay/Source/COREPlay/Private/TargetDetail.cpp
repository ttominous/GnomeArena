#include "TargetDetail.h"
#include "Creature.h"
#include "SceneObject.h"
#include "DropItem.h"
#include "XMLTools.h"
#include "StringTools.h"

FTargetDetail::FTargetDetail(UObject* object, FVector inLocation) {
	type = ETargetClass::LOCATION;
	setObject(object);
	location = inLocation;
}

FTargetDetail::FTargetDetail(UObject* object) {
	setObject(object);
}

FTargetDetail::FTargetDetail(FVector inLocation) {
	location = inLocation;
	type = ETargetClass::LOCATION;
}

FTargetDetail::FTargetDetail(FHitResult hit) {
	setObject(hit.GetActor());
	location = hit.Location;

	if (hit.GetComponent()) {
		address.Add(hit.GetComponent()->GetName());
		if (hit.BoneName != "") {
			address.Add(hit.BoneName.ToString());
		}
	}
}

FString FTargetDetail::getAddressString() {
	FString result = address.Num() > 0 ? address[0] : "";
	for (int i = 1; i < address.Num(); i++) {
		result += "." + address[i];
	}
	return result;
}

void FTargetDetail::setObject(UObject* object) {
	if (object == nullptr) {
		return;
	}
	address.Empty();

	if (object->IsA(ACreature::StaticClass())) {
		ACreature* creature = Cast<ACreature>(object);
		type = ETargetClass::CREATURE;
		address.Add(FString::FromInt(creature->id));
		location = creature->GetActorLocation();
	}
	else if (object->IsA(ASceneObject::StaticClass())) {
		ASceneObject* sceneObject = Cast<ASceneObject>(object);
		type = ETargetClass::SCENE_OBJECT;
		address.Add(FString::FromInt(sceneObject->id));
		location = sceneObject->GetActorLocation();
	}
	else if (object->IsA(ADropItem::StaticClass())) {
		ADropItem* dropItem = Cast<ADropItem>(object);
		type = ETargetClass::DROP_ITEM;
		address.Add(FString::FromInt(dropItem->id));
		location = dropItem->GetActorLocation();
	}
	else if (object->IsA(AActor::StaticClass())) {
		AActor* actor = Cast< AActor >(object);
		if (actor->Tags.Contains("Terrain")) {
			type = ETargetClass::TERRAIN;
		}
	}
}

ACreature* FTargetDetail::getCreature(UWorld* world) {
	if (world == nullptr || type != ETargetClass::CREATURE || address.Num() == 0) {
		return nullptr;
	}
	int id = FCString::Atoi(*address[0]);
	return ACreature::getByID(id);
}

ASceneObject* FTargetDetail::getSceneObject(UWorld* world) {
	if (world == nullptr || type != ETargetClass::SCENE_OBJECT || address.Num() == 0) {
		return nullptr;
	}
	int id = FCString::Atoi(*address[0]);
	return ASceneObject::getByID(id);
}

ADropItem* FTargetDetail::getDropItem(UWorld* world) {
	if (world == nullptr || type != ETargetClass::DROP_ITEM || address.Num() == 0) {
		return nullptr;
	}
	int id = FCString::Atoi(*address[0]);
	return ADropItem::getByID(id);
}

AActor* FTargetDetail::getActor(UWorld* world) {
	if (type == ETargetClass::CREATURE) {
		return getCreature(world);
	}
	if (type == ETargetClass::SCENE_OBJECT) {
		return getSceneObject(world);
	}
	if (type == ETargetClass::DROP_ITEM) {
		return getDropItem(world);
	}
	return nullptr;
}


FString FTargetDetail::getComponentName() {
	if ( (type != ETargetClass::CREATURE && type != ETargetClass::SCENE_OBJECT) || address.Num() < 2) {
		return "";
	}
	return address[1];
}

void FTargetDetail::setComponentName(FString componentName) {
	if (type != ETargetClass::CREATURE || address.Num() == 0) {
		return;
	}
	if (address.Num() < 2) {
		address.Add(componentName);
	}
	else {
		address[1] = componentName;
	}
}

FName FTargetDetail::getHitBoneName() {
	if (type != ETargetClass::CREATURE || address.Num() < 3) {
		return "";
	}
	return FName(*address[2]);
}

void FTargetDetail::setHitBoneName(FString boneName) {
	if (type != ETargetClass::CREATURE || address.Num() == 0) {
		return;
	}
	if (address.Num() < 2) {
		address.Add("");
		address.Add(boneName);
	}
	else if (address.Num() < 3) {
		address.Add(boneName);
	}
	else {
		address[2] = boneName;
	}
}

bool FTargetDetail::isEmpty() {
	return type == ETargetClass::UNKNOWN;
}

bool FTargetDetail::isPopulated() {
	return type != ETargetClass::UNKNOWN;
}

bool FTargetDetail::same(FTargetDetail& otherDetail) {
	if (address.Num() != otherDetail.address.Num()) {
		return false;
	}
	for (int i = 0; i < address.Num(); i++) {
		if (!address[i].Equals(otherDetail.address[i])) {
			return false;
		}
	}
	return true;
}

FVector FTargetDetail::getCurrentLocation(UWorld* world) {
	if (type == ETargetClass::CREATURE) {
		ACreature* creature = getCreature(world);
		if (creature) {
			return creature->GetActorLocation();
		}
	}
	else if (type == ETargetClass::SCENE_OBJECT) {
		ASceneObject* sceneObject = getSceneObject(world);
		if (sceneObject) {
			return sceneObject->GetActorLocation();
		}
	}
	else if (type == ETargetClass::DROP_ITEM) {
		ADropItem* dropItem = getDropItem(world);
		if (dropItem) {
			return dropItem->GetActorLocation();
		}
	}
	return location;
}

float FTargetDetail::getRadius(UWorld* world) {
	if (type == ETargetClass::CREATURE) {
		ACreature* creature = getCreature(world);
		if (creature) {
			return creature->getCapsuleRadius();
		}
	}
	return 0.0f;
}

float FTargetDetail::getHalfHeight(UWorld* world) {
	if (type == ETargetClass::CREATURE) {
		ACreature* creature = getCreature(world);
		if (creature) {
			return creature->getCapsuleHalfHeight();
		}
	}
	return 0.0f;
}


void FTargetDetail::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type",(double)type);
	rootNode.setAttribute("location", location);
	rootNode.setAttribute("address", getAddressString());
}

FTargetDetail FTargetDetail::fromXML(FXmlNode* rootNode) {
	FTargetDetail result;
	if (rootNode != nullptr) {
		result.type = (ETargetClass)UXMLTools::getAttributeInt(rootNode, "type");
		result.address = UStringTools::parseList(UXMLTools::getAttributeString(rootNode, "address"), ".");
		result.location = UXMLTools::getAttributeVector(rootNode, "location");
	}
	return result;
}