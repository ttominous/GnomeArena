#pragma once

#include "CoreMinimal.h"

#include "DecalSpec.generated.h"

UCLASS(Blueprintable)
class COREPLAY_API UDecalSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		TArray< UMaterial* > materials;

	UPROPERTY(EditAnywhere)
		float lifeSpan = 10.0f;

	UPROPERTY(EditAnywhere)
		float size = 100.0f;

	UPROPERTY(EditAnywhere)
		int minVisualEffectQualityLevel = 0;
};
