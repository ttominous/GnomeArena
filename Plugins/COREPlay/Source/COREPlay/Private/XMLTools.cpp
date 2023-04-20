#include "XMLTools.h"
#include "FileTools.h"
#include "StringTools.h"
#include "Debug.h"

FXmlNode* UXMLTools::getXMLRoot(FString path, bool includeContentPath) {
	if (includeContentPath) {
		FString rootPath = UFileTools::getRootContentPath();
		if (!path.StartsWith(rootPath)) {
			path = rootPath + path;
		}
	}
	FXmlFile* file = new FXmlFile(path);
	return file->GetRootNode();
}

TArray<FXmlNode*> UXMLTools::parseXMLFile(FString path, bool includeContentPath) {

	if (includeContentPath) {
		FString rootPath = UFileTools::getRootContentPath();
		if (!path.StartsWith(rootPath)) {
			path = rootPath + path;
		}
	}

	FXmlFile* file = new FXmlFile(path);

	FXmlNode* rootNode = file->GetRootNode();
	TArray< FXmlNode* > childNodes;

	if (rootNode) {

		if (rootNode->GetTag().ToUpper().Equals("ROOT")) {
			childNodes = rootNode->GetChildrenNodes();
			if (childNodes.Num() == 0) {
				childNodes.Add(rootNode);
			}
		}
		else {
			childNodes.Add(rootNode);
		}
	}
	else {
		UDebug::error("Could not find XML nodes in " + path);
		UDebug::error(file->GetLastError());
	}
	return childNodes;
}

FString UXMLTools::getNodeValue(FXmlNode* node, FString tag) {

	FXmlNode* childNode = node->FindChildNode(tag);
	if (childNode) {
		return node->GetContent();
	}

	return "";
}

int UXMLTools::getAttributeInt(FXmlNode* node, FString attribute, int defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		if (attributeValue.Contains(".")) {
			float floatValue = FCString::Atof(*attributeValue);
			return FMath::RoundToInt(floatValue);
		}
		return FCString::Atoi(*attributeValue);
	}
	return defaultValue;
}

float UXMLTools::getAttributeFloat(FXmlNode* node, FString attribute, float defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		return FCString::Atof(*attributeValue);
	}
	return defaultValue;
}

double UXMLTools::getAttributeDouble(FXmlNode* node, FString attribute, float defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		return (double)FCString::Atof(*attributeValue);
	}
	return defaultValue;
}

FString UXMLTools::getAttributeString(FXmlNode* node, FString attribute, FString defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		return attributeValue;
	}
	return defaultValue;
}

bool UXMLTools::getAttributeBool(FXmlNode* node, FString attribute, bool defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		return attributeValue.ToUpper().Equals("TRUE");
	}
	return defaultValue;
}

FVector UXMLTools::getAttributeVector(FXmlNode* node, FString attribute, FVector defaultValue) {

	TArray< FString > values = UStringTools::parseList(node->GetAttribute(attribute), "|");

	defaultValue.X = values.Num() > 0 ? FCString::Atof(*values[0]) : defaultValue.X;
	defaultValue.Y = values.Num() > 1 ? FCString::Atof(*values[1]) : defaultValue.Y;
	defaultValue.Z = values.Num() > 2 ? FCString::Atof(*values[2]) : defaultValue.Z;
	return defaultValue;
}

FLinearColor UXMLTools::getAttributeLinearColor(FXmlNode* node, FString attribute, FLinearColor defaultValue) {

	TArray< FString > values = UStringTools::parseList(node->GetAttribute(attribute), "|");

	defaultValue.R = values.Num() > 0 ? FCString::Atof(*values[0]) : defaultValue.R;
	defaultValue.G = values.Num() > 1 ? FCString::Atof(*values[1]) : defaultValue.G;
	defaultValue.B = values.Num() > 2 ? FCString::Atof(*values[2]) : defaultValue.B;
	defaultValue.A = values.Num() > 3 ? FCString::Atof(*values[3]) : defaultValue.A;

	return defaultValue;
}

FColor UXMLTools::getAttributeColor(FXmlNode* node, FString attribute, FColor defaultValue) {
	TArray< FString > values = UStringTools::parseList(node->GetAttribute(attribute), "|");

	defaultValue.R = values.Num() > 0 ? FCString::Atof(*values[0]) : defaultValue.R;
	defaultValue.G = values.Num() > 1 ? FCString::Atof(*values[1]) : defaultValue.G;
	defaultValue.B = values.Num() > 2 ? FCString::Atof(*values[2]) : defaultValue.B;
	defaultValue.A = values.Num() > 3 ? FCString::Atof(*values[3]) : defaultValue.A;

	return defaultValue;
}


FRotator UXMLTools::getAttributeRotator(FXmlNode* node, FString attribute, FRotator defaultValue) {

	TArray< FString > values = UStringTools::parseList(node->GetAttribute(attribute), "|");

	defaultValue.Pitch = values.Num() > 0 ? FCString::Atof(*values[0]) : defaultValue.Pitch;
	defaultValue.Yaw = values.Num() > 1 ? FCString::Atof(*values[1]) : defaultValue.Yaw;
	defaultValue.Roll = values.Num() > 2 ? FCString::Atof(*values[2]) : defaultValue.Roll;
	return defaultValue;
}


bool UXMLTools::hasAttribute(FXmlNode* node, FString attribute) {
	return node->GetAttribute(attribute).Len() > 0;
}

FString UXMLTools::getAttributeString(TMap< FString, FString > attributeValues) {
	FString result = "";
	int count = 0;
	for (auto& attributeEntry : attributeValues) {
		if (count > 0) {
			result += " ";
		}
		result += (attributeEntry.Key + "=\"" + attributeEntry.Value + "\"");
		count++;
	}
	return result;
}

UClass* UXMLTools::getAttributeClass(FXmlNode* node, FString attribute, UClass* defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() == 0) {
		return defaultValue;
	}
	UClass* result = LoadClass< UObject >(NULL, *attributeValue, NULL, LOAD_None, NULL);
	return result == nullptr ? defaultValue : result;
}

TArray< FString > UXMLTools::getAttributeStringList(FXmlNode* node, FString attribute, TArray< FString > defaultValue) {
	FString attributeValue = node->GetAttribute(attribute);
	if (attributeValue.Len() > 0) {
		return UStringTools::parseList(node->GetAttribute(attribute), "|");
	}
	return defaultValue;
}

FXmlFile* UXMLTools::createNewFile() {
	FXmlFile* xmlFile = new FXmlFile("<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n<root>\n</root>", EConstructMethod::ConstructFromBuffer);
	return xmlFile;
}

FXmlNode* UXMLTools::addChildNode(FXmlNode* parent, FString tag) {
	if (parent == nullptr) {
		UDebug::error("Failed to add XML node " + tag + "! Parent node is missing.");
		return nullptr;
	}
	int childCount = parent->GetChildrenNodes().Num();
	parent->AppendChildNode(tag, "");
	return parent->GetChildrenNodes()[childCount];
}

FXmlNode* UXMLTools::getFirstChild(FXmlNode* parent, FString tag) {
	tag = tag.ToUpper();
	TArray< FXmlNode* > childNodes = parent->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString childTag = childNode->GetTag().ToUpper();
		if (childTag.Equals(tag)) {
			return childNode;
		}
	}
	return nullptr;
}





FString FXMLBuilderNode::toString(FString indent) {

	FString result = indent + "<" + tag;

	if (attributes.Num() > 0) {
		for (auto attribute : attributes) {
			result += " " + attribute.Key + "=\"" + attribute.Value + "\"";
		}
	}

	if (children.Num() == 0) {
		result += " />\n";
	}
	else {
		result += ">\n";
		for (auto& child : children) {
			result += child.toString(indent + "\t");
		}
		result += indent + "</" + tag + ">\n";
	}

	return result;
}

FXMLBuilderNode& FXMLBuilderNode::addChild(FString childTag) {
	FXMLBuilderNode node;
	node.tag = childTag;
	children.Add(node);
	return children[children.Num() - 1];
}

void FXMLBuilderNode::setAttribute(FString key, FString value) {
	attributes.Add(key, value);
}

void FXMLBuilderNode::setAttribute(FString key, double value) {
	attributes.Add(key, FString::SanitizeFloat(value));
}

void FXMLBuilderNode::setAttribute(FString key, bool value) {
	attributes.Add(key, (value ? "true" : "false"));
}

void FXMLBuilderNode::setAttribute(FString key, FVector value) {
	attributes.Add(key, FString::SanitizeFloat(value.X) + "|" + FString::SanitizeFloat(value.Y) + "|" + FString::SanitizeFloat(value.Z));
}

void FXMLBuilderNode::setAttribute(FString key, FColor value) {
	attributes.Add(key, FString::SanitizeFloat(value.R) + "|" + FString::SanitizeFloat(value.G) + "|" + FString::SanitizeFloat(value.B) + "|" + FString::SanitizeFloat(value.A));
}

void FXMLBuilderNode::setAttribute(FString key, FName value) {
	attributes.Add(key, value.ToString());
}

void FXMLBuilderNode::setAttribute(FString key, FRotator value) {
	attributes.Add(key, FString::SanitizeFloat(value.Pitch) + "|" + FString::SanitizeFloat(value.Yaw) + "|" + FString::SanitizeFloat(value.Roll));
}

void FXMLBuilderNode::setAttribute(FString key, TArray<FString> values) {
	FString finalValue = "";
	for (int i = 0; i < values.Num(); i++) {
		if (i > 0) {
			finalValue += "|";
		}
		finalValue += values[i].Replace(TEXT("|"), TEXT(""));
	}
	attributes.Add(key, finalValue);
}


FXMLBuilder::FXMLBuilder() {
	root.tag = "root";
}

FString FXMLBuilder::toString() {
	return "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n" + root.toString("");
}


FXmlFile* FXMLBuilder::getXMLFile() {
	return new FXmlFile(toString(), EConstructMethod::ConstructFromBuffer);
}

FXMLBuilderNode& FXMLBuilder::addNode(FString tag) {
	FXMLBuilderNode node;
	node.tag = tag;
	root.children.Add(node);
	return root.children[root.children.Num() - 1];
}