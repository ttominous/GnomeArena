#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "MaterialPicker.generated.h"


USTRUCT()
struct COREPLAY_API FMaterialPickerOption {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UMaterialInterface* material;

	UPROPERTY(EditAnywhere)
		TMap<FString, int> conditions;
};

UCLASS()
class COREPLAY_API UMaterialPicker : public UDataAsset {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UMaterialInterface* defaultMaterial;

	UPROPERTY(EditAnywhere)
		TArray< FMaterialPickerOption > optionalMaterials;


	virtual UMaterialInterface* getMaterial(TMap<FString, int>& materialSettings);

};
