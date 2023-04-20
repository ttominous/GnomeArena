#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MindComponent.h"
#include "DualAxisCreature.h"


#include "GnomeMindComponent.generated.h"

UENUM(BlueprintType)
enum class EStrafeType : uint8 {
    NONE = 0                      UMETA(DisplayName = "None"),
    LEFT = 1                      UMETA(DisplayName = "Left"),
    RIGHT = 2                     UMETA(DisplayName = "Right"),
    BACK = 3                      UMETA(DisplayName = "Back"),
    FORWARD = 4                   UMETA(DisplayName = "Forward"),
    JUMP = 5                      UMETA(DisplayName = "Jump")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GNOMEARENA_API UGnomeMindComponent : public UMindComponent {
	GENERATED_BODY()

public:

    UPROPERTY()
        EStrafeType strafeType = EStrafeType::NONE;

    UPROPERTY()
        float strafeTimer = 1.0f;

    UPROPERTY()
        float primaryBurstTime = -1.0f;

    UPROPERTY()
        float primaryAttackCooldown = -1.0f;

    UPROPERTY()
        float secondaryAttackCooldown = -1.0f;

	virtual void update(ACreature* creature, float deltaTime) override;
	virtual void updateTarget(ADualAxisCreature* creature, float deltaTime);

};