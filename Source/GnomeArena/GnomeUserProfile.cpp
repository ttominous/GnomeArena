#include "GnomeUserProfile.h"
#include "JsonTools.h"
#include "Debug.h"


void UGnomeUserProfile::writeXML(FXMLBuilderNode& rootNode) {
	Super::writeXML(rootNode);
	rootNode.setAttribute("defaultGnomeColor", defaultGnomeColor);
}

void UGnomeUserProfile::readXML(FXmlNode* rootNode) {
	Super::readXML(rootNode);
	defaultGnomeColor = UXMLTools::getAttributeColor(rootNode, "defaultGnomeColor");
}

void UGnomeUserProfile::parseExtraData(FString extraData) {
	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(extraData);
	if (root == nullptr) {
		return;
	}
	TSharedPtr<FJsonObject> gnomeArena = UJsonTools::getObject(root, "GnomeArena");
	if (gnomeArena == nullptr) {
		return;
	}

	defaultGnomeColor = UJsonTools::getColor(gnomeArena, "defaultGnomeColor");
}