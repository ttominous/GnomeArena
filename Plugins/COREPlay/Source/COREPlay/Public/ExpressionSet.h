#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ExpressionSet.generated.h"

USTRUCT()
struct COREPLAY_API FExpression {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, float > morphs;

	UPROPERTY(EditAnywhere)
		float microExpressionAmount = 0.0f;

};


UCLASS()
class COREPLAY_API UExpressionSet : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, FExpression > expressions;

	UPROPERTY(EditAnywhere)
		TArray< FString > microExpressions;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > expressionKeys;

	virtual FString getExpressionKey(FString key);
};