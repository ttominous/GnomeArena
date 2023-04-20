#pragma once

#include "CoreMinimal.h"
#include "PlayAnimAction.h"

#include "PumpWeaponAction.generated.h"

UCLASS()
class COREPLAY_API UPumpWeaponAction : public UPlayAnimAction {

	GENERATED_BODY()

public:

	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) override;
	virtual void processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) override;

};
