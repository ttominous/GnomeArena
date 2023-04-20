#pragma once

#include "CoreMinimal.h"
#include "FXList.h"
#include "DecalSpec.h"
#include "ItemSet.h"
#include "AssetType.h"

#include "SceneObjectSpec.generated.h"

class ASceneObject;
class UVesselSpec;

UCLASS(Blueprintable)
class COREPLAY_API USceneObjectSpec : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString displayName;

	UPROPERTY(EditAnywhere)
		FString iconKey;

	UPROPERTY(EditAnywhere)
		float iconArmLength = 500.0f;

	UPROPERTY(EditAnywhere)
		FVector iconArmOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
		FRotator iconArmRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
		TArray< FAssetSpec > assets;

	UPROPERTY(EditAnywhere)
		UVesselSpec* vesselSpec = nullptr;

	UPROPERTY(EditAnywhere)
		FVector collisionBounds;

	UPROPERTY(EditAnywhere)
		bool blockNavmesh = true;

	UPROPERTY(EditAnywhere)
		TSubclassOf< ASceneObject > sceneObjectClass;

};
