#pragma once

#include "CoreMinimal.h"
#include "UserProfile.h"
#include "XMLTools.h"

#include "GnomeUserProfile.generated.h"

UCLASS(BlueprintType)
class GNOMEARENA_API UGnomeUserProfile : public UUserProfile {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
		FColor defaultGnomeColor = FColor::Blue;

	virtual void writeXML(FXMLBuilderNode& rootNode) override;
	virtual void readXML(FXmlNode* rootNode) override;
	virtual void parseExtraData(FString extraData) override;

};