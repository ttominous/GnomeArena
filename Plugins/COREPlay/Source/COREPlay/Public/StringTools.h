#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Internationalization/Regex.h"

#include "StringTools.generated.h"


UCLASS()
class COREPLAY_API UStringTools : public UObject {

	GENERATED_BODY()

public:


	static FORCEINLINE FString twoDecimalString(float value) {
		FNumberFormattingOptions format;
		format.MinimumIntegralDigits = 1;
		format.MaximumIntegralDigits = 10000;
		format.MinimumFractionalDigits = 0;
		format.MaximumFractionalDigits = 2;
		return FText::AsNumber(value, &format).ToString();
	}

	static TArray< FString > parseList(FString value, FString delimiter);
	static TArray< FName > parseNameList(FString value, FString delimiter);
	static FVector parseVector(FString vectorString, FString delimiter);
	static FString left(FString value, FString delimiter);
	static FString right(FString value, FString delimiter);
	static FString getRandom(TArray< FString >& values);
	static bool matchesPattern(FString value, FString pattern);
	static FString defaultString(FString value, FString defaultValue = "");
	static FString fromBool(bool value);
	static FString getAssetPathString(UObject* object);
	static bool isEmpty(FString value);
	static FString escapeQuotes(FString value);
	static bool containsIgnoreCase(FString value, FString searchString);

};