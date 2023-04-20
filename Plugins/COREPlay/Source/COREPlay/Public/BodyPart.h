#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AssetType.h"
#include "MaterialPicker.h"

#include "BodyPart.generated.h"

class UBodyComponent;

USTRUCT()
struct COREPLAY_API FBodyPartPhysics {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString bone;

	UPROPERTY(EditAnywhere)
		float amount = 1.0f;

	UPROPERTY(EditAnywhere)
		bool simulate = false;

	UPROPERTY(EditAnywhere)
		bool includeSelf = false;
};

USTRUCT()
struct COREPLAY_API FBodyPartMaterial {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString slot;

	UPROPERTY(EditAnywhere)
		TArray< FString > relevantParameters;

	UPROPERTY(EditAnywhere)
		TMap< FString, double > scalarParameters;

	UPROPERTY(EditAnywhere)
		TMap< FString, FVector > vectorParameters;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > textureParameters;

	UPROPERTY(EditAnywhere)
		TMap< FString, FString > parameterForwarding;

	UPROPERTY(EditAnywhere)
		UMaterialPicker* lodPicker = nullptr;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* materialOverride = nullptr;

	UPROPERTY()
		UMaterialInstance* sharedMaterial = nullptr;

	UPROPERTY()
		int slotIndex = -1;

	bool instanced();
	UMaterialInstance* getMaterialInstance(UMeshComponent* meshComponent);

};

USTRUCT()
struct COREPLAY_API FBodyPartDetail {

	GENERATED_BODY()

public:

	static FBodyPartDetail EMPTY_DETAILS;

	UPROPERTY(EditAnywhere)
		UObject* asset;

	UPROPERTY(EditAnywhere)
		bool useRootSkeletonAnimation = true;

	UPROPERTY(EditAnywhere)
		FTransform transform;

	UPROPERTY(EditAnywhere)
		FString attachToSlot;

	UPROPERTY(EditAnywhere)
		TArray< FBodyPartPhysics > physics;

	UPROPERTY(EditAnywhere)
		TArray< FBodyPartMaterial > materials;

	UPROPERTY(EditAnywhere)
		TArray< FString > relevantMorphs;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > morphs;

	UPROPERTY(EditAnywhere)
		TArray< FString > tags;

	UPROPERTY(EditAnywhere)
		FString collisionEnabled = "NoCollision";

	UPROPERTY(EditAnywhere)
		FString collisionChannel = "PhysicsBody";

	UPROPERTY(EditAnywhere)
		TArray< FString > physicsMorphSlots;

	EAssetType getType();
	UStaticMesh* getStaticMesh();
	USkeletalMesh* getSkeletalMesh();

};


UCLASS()
class COREPLAY_API UBodyPart : public UDataAsset {

	GENERATED_BODY()

public:

	virtual FBodyPartDetail& getDetails(FString slotKey, UBodyComponent* component);

};


UCLASS()
class COREPLAY_API USingleBodyPart : public UBodyPart {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FBodyPartDetail defaultDetails;

	virtual FBodyPartDetail& getDetails(FString slotKey, UBodyComponent* component) override;

};


UCLASS()
class COREPLAY_API UConditionalBodyPart : public USingleBodyPart {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, FBodyPartDetail > variations;

	virtual FString getKey(FString slotKey, UBodyComponent* component);
	virtual FBodyPartDetail& getDetails(FString slotKey, UBodyComponent* component) override;

};
