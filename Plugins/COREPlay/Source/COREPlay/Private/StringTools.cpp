#include "StringTools.h"
#include "Debug.h"

TArray< FString > UStringTools::parseList(FString value, FString delimiter) {
	TArray< FString > result;
	value.ParseIntoArray(result, *delimiter, true);
	return result;
}

TArray< FName > UStringTools::parseNameList(FString value, FString delimiter) {
	TArray< FString > strings = UStringTools::parseList(value, delimiter);
	TArray< FName > result;
	for (FString string : strings) {
		result.Add(FName(*string));
	}
	return result;
}

FVector UStringTools::parseVector(FString vectorString, FString delimiter) {
	TArray< FString > strings = UStringTools::parseList(vectorString, delimiter);
	FVector result = FVector::ZeroVector;
	if (strings.Num() > 0) {
		result.X = FCString::Atof(*strings[0]);
		if (strings.Num() > 1) {
			result.Y = FCString::Atof(*strings[1]);
			if (strings.Num() > 2) {
				result.Z = FCString::Atof(*strings[2]);
			}
		}
	}
	return result;
}

FString UStringTools::left(FString value, FString delimiter) {
	FString leftValue, rightValue;
	value.Split(*delimiter, &leftValue, &rightValue);
	return leftValue;
}

FString UStringTools::right(FString value, FString delimiter) {
	FString leftValue, rightValue;
	value.Split(*delimiter, &leftValue, &rightValue);
	return rightValue;
}

FString UStringTools::getRandom(TArray< FString >& values) {
	if (values.Num() > 0) {
		return values[FMath::RandRange(0, values.Num() - 1)];
	}
	return "";
}

bool UStringTools::matchesPattern(FString value, FString pattern) {
	FRegexPattern regexPattern(*value);
	FRegexMatcher matcher(regexPattern, *value);
	return matcher.FindNext();
}

FString UStringTools::defaultString(FString value, FString defaultValue) {
	return value.Len() == 0 ? defaultValue : value;
}

FString UStringTools::fromBool(bool value) {
	return value ? "true" : "false";
}

FString UStringTools::getAssetPathString(UObject* object) {
	if (object == nullptr) {
		return "";
	}
	FSoftObjectPath assetReference = FSoftObjectPath(object);
	return assetReference.GetLongPackageName();
}

bool UStringTools::isEmpty(FString value) {
	return value.Replace(TEXT(" "), TEXT(""), ESearchCase::CaseSensitive).Len() == 0;
}

FString UStringTools::escapeQuotes(FString value) {
	return value.ReplaceQuotesWithEscapedQuotes();
}

bool UStringTools::containsIgnoreCase(FString value, FString searchString) {
	return value.Contains(*searchString, ESearchCase::IgnoreCase, ESearchDir::FromStart);
}