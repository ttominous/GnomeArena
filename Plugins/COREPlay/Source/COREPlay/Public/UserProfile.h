#pragma once

#include "CoreMinimal.h"
#include "XMLTools.h"

#include "UserProfile.generated.h"

UCLASS()
class COREPLAY_API UUserProfile : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString username;

	UPROPERTY()
		FString password;

	UPROPERTY()
		bool local;

	UPROPERTY()
		FString token;

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);
	virtual void parseExtraData(FString extraData);
};