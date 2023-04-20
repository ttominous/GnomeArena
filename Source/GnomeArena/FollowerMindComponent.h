#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MindComponent.h"
#include "DualAxisCreature.h"


#include "FollowerMindComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GNOMEARENA_API UFollowerMindComponent : public UMindComponent {
    GENERATED_BODY()

public:

    virtual void update(ACreature* creature, float deltaTime) override;

};