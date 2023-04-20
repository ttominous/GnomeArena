#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"

#include "CommonNavigationLinks.generated.h"


UENUM(BlueprintType)
enum class ENavPathNodeType : uint8 {
	JUMP = 0                   UMETA(DisplayName = "Jump"),
	WALK = 1                   UMETA(DisplayName = "Walk")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UComplexNavLinkComponent : public UNavLinkCustomComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		ENavPathNodeType pathType;

};


USTRUCT(BlueprintType)
struct COREPLAY_API FNavPathNode {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UComplexNavLinkComponent* link = nullptr;

	UPROPERTY(EditAnywhere)
		FVector location;

};