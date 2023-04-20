#pragma once

#include "CoreMinimal.h"
#include "PlayAnimAction.h"

#include "ReloadWeaponAction.generated.h"

UCLASS()
class COREPLAY_API UReloadWeaponAction : public UPlayAnimAction {

	GENERATED_BODY()

public:

	virtual bool canDoAction(AActor* actor) override;
	virtual void processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) override;
	virtual void processAnimationFinished(UActionInstance* instance, UActionComponent* component) override;
};
