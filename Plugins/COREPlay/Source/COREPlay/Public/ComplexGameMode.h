#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ComplexGameStage.h"
#include "TeamSpec.h"

#include "ComplexGameMode.generated.h"


UCLASS()
class COREPLAY_API AComplexGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		EComplexGameStage gameStage;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION(Exec, Category = ExecFunctions)
		virtual void SpawnCreature(const FString& specKey);

	UFUNCTION(Exec, Category = ExecFunctions)
		virtual void RemoveCreature();

};
