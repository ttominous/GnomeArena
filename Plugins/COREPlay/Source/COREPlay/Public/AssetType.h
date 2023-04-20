#pragma once

#include "CoreMinimal.h"

#include "AssetType.generated.h"

UENUM(BlueprintType)
enum class EAssetType : uint8 {
    UNKNOWN = 0                   UMETA(DisplayName = "Unknown"),
    NIAGARA_SYSTEM = 1            UMETA(DisplayName = "NiagaraSystem"),
    SOUND = 2                     UMETA(DisplayName = "Sound"),
    STATIC_MESH = 3               UMETA(DisplayName = "StaticMesh"),
    SKELETAL_MESH = 4             UMETA(DisplayName = "SkeletalMesh")
};


USTRUCT()
struct COREPLAY_API FAssetType {
    GENERATED_BODY()

public:

    static EAssetType getType(UObject* object);
};

USTRUCT()
struct COREPLAY_API FAssetSpec {
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere)
        EAssetType type;

    UPROPERTY(EditAnywhere)
        UObject* asset = nullptr;

    UPROPERTY(EditAnywhere)
        FTransform transform;
};