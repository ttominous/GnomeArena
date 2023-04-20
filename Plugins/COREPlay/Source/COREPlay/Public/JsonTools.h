#pragma once

#include "CoreMinimal.h"
#include "Serialization/JsonSerializer.h"

#include "JsonTools.generated.h"

UCLASS()
class COREPLAY_API UJsonTools : public UObject {

	GENERATED_BODY()

public:

	static TSharedPtr<FJsonObject> parseJSON(FString jsonString);
	static TArray< TSharedPtr<FJsonValue > > parseJSONArray(FString jsonString);

	static FString toString(TSharedPtr<FJsonObject> root, bool escapeQuotes);

	static TSharedPtr<FJsonObject> makeObject();

	static void setObject(TSharedPtr<FJsonObject> parent, FString key, TSharedPtr<FJsonObject> child);
	static TSharedPtr<FJsonObject> getObject(TSharedPtr<FJsonObject> parent, FString key);

	static void setObjectArray(TSharedPtr<FJsonObject> parent, FString key, TArray< TSharedPtr<FJsonObject> > children);
	static TArray < TSharedPtr<FJsonObject> > getObjectArray(TSharedPtr<FJsonObject> parent, FString key);


	static void setString(TSharedPtr<FJsonObject> parent, FString key, FString value);
	static FString getString(TSharedPtr<FJsonObject> parent, FString key);

	static void setColor(TSharedPtr<FJsonObject> parent, FString key, FColor value);
	static FColor getColor(TSharedPtr<FJsonObject> parent, FString key);

	static void setNumeric(TSharedPtr<FJsonObject> parent, FString key, double value);
	static double getNumeric(TSharedPtr<FJsonObject> parent, FString key);

};