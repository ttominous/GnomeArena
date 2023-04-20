#pragma once

#include "XmlParser.h"
#include "StringTools.h"

#include "XMLTools.generated.h"

UCLASS()
class COREPLAY_API UXMLTools : public UObject {

	GENERATED_BODY()

public:

	static FXmlNode* getXMLRoot(FString path, bool includeContentPath = true);
	static TArray<FXmlNode*> parseXMLFile(FString path, bool includeContentPath = true);
	static FString getNodeValue(FXmlNode* node, FString tag);
	static int getAttributeInt(FXmlNode* node, FString attribute, int defaultValue = 0);
	static float getAttributeFloat(FXmlNode* node, FString attribute, float defaultValue = 0.0f);
	static double getAttributeDouble(FXmlNode* node, FString attribute, float defaultValue = 0.0);
	static FString getAttributeString(FXmlNode* node, FString attribute, FString defaultValue = "");
	static bool getAttributeBool(FXmlNode* node, FString attribute, bool defaultValue = false);
	static FVector getAttributeVector(FXmlNode* node, FString attribute, FVector defaultValue = FVector(0, 0, 0));
	static FLinearColor getAttributeLinearColor(FXmlNode* node, FString attribute, FLinearColor defaultValue = FLinearColor(0, 0, 0));
	static FColor getAttributeColor(FXmlNode* node, FString attribute, FColor defaultValue = FColor(0,0,0));
	static FRotator getAttributeRotator(FXmlNode* node, FString attribute, FRotator defaultValue = FRotator(0, 0, 0));
	static UClass* getAttributeClass(FXmlNode* node, FString attribute, UClass* defaultValue = nullptr);
	static TArray< FString > getAttributeStringList(FXmlNode* node, FString attribute, TArray< FString > defaultValue);


	static bool hasAttribute(FXmlNode* node, FString attribute);

	static FString getAttributeString(TMap< FString, FString > attributeValues);

	static FXmlFile* createNewFile();
	static FXmlNode* addChildNode(FXmlNode* parent, FString tag);
	static FXmlNode* getFirstChild(FXmlNode* parent, FString tag);

};


struct COREPLAY_API FXMLBuilderNode {

public:

	FString tag;
	TMap <FString, FString> attributes;
	TArray< FXMLBuilderNode > children;

	FString toString(FString indent);
	FXMLBuilderNode& addChild(FString childTag);
	void setAttribute(FString key, FString value);
	void setAttribute(FString key, double value);
	void setAttribute(FString key, bool value);
	void setAttribute(FString key, FVector value);
	void setAttribute(FString key, FColor value);
	void setAttribute(FString key, FRotator value);
	void setAttribute(FString key, FName value);
	void setAttribute(FString key, TArray<FString> values);

};

struct COREPLAY_API FXMLBuilder {

public:

	FXMLBuilderNode root;

	FXMLBuilder();
	FString toString();
	FXmlFile* getXMLFile();

	FXMLBuilderNode& addNode(FString tag);

};