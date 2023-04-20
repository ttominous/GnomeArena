#include "UserProfile.h"
#include "Debug.h"


void UUserProfile::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("username", username);
	rootNode.setAttribute("password", password);
	rootNode.setAttribute("local", local);
	rootNode.setAttribute("token", token);
}

void UUserProfile::readXML(FXmlNode* rootNode) {
	username = UXMLTools::getAttributeString(rootNode, "username");
	password = UXMLTools::getAttributeString(rootNode, "password");
	local = UXMLTools::getAttributeBool(rootNode, "local");
	token = UXMLTools::getAttributeString(rootNode, "token");
}

void UUserProfile::parseExtraData(FString extraData) {

}