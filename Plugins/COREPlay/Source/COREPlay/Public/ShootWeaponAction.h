#pragma once

#include "CoreMinimal.h"
#include "PlayAnimAction.h"

#include "ShootWeaponAction.generated.h"

UCLASS()
class COREPLAY_API UShootWeaponAction : public UPlayAnimAction {

	GENERATED_BODY()

public:

	virtual bool canDoAction(AActor* actor) override;
	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) override;
	virtual void processAnimationFinished(UActionInstance* instance, UActionComponent* component) override;

};
