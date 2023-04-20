#include "ExpressionSet.h"

FString UExpressionSet::getExpressionKey(FString key) {
	return expressionKeys.Contains(key) ? expressionKeys[key] : key;
}