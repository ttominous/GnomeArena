#pragma once

#include "CoreMinimal.h"
#include "TwoEntityAction.h"
#include "PlayAnimationData.h"

#include "TwoEntityPlayAnimAction.generated.h"

class UActionInstance;
class UActionComponent;

UCLASS()
class COREPLAY_API UTwoEntityPlayAnimAction : public UTwoEntityAction {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString leaderDefaultAnimKey = "Default";

	UPROPERTY(EditAnywhere)
		TMap< FString, FPlayAnimationData > leaderAnimations;

	UPROPERTY(EditAnywhere)
		FString followerDefaultAnimKey = "Default";

	UPROPERTY(EditAnywhere)
		TMap< FString, FPlayAnimationData > followerAnimations;

	virtual void processUpdate(float deltaTime, UActionInstance* instance, UActionComponent* component) override;

	virtual void processLeaderAnimationFinished(UActionInstance* instance, UActionComponent* component) override;
	virtual void processFollowerAnimationFinished(UActionInstance* instance, UActionComponent* component) override;

	virtual void processAnimationsFinished(UActionInstance* instance, UActionComponent* component);

	virtual void processActualEnd(UActionInstance* instance, UActionComponent* component) override;


	virtual void assignAnimKeys(UActionInstance* instance, UActionComponent* component);
	virtual UClass* getInstanceClass(UActionComponent* component) override;

};



UCLASS(Blueprintable)
class COREPLAY_API UTwoEntityPlayAnimActionInstance : public UTwoEntityActionInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool animationsStarted = false;

	UPROPERTY()
		bool leaderAnimFinished = false;

	UPROPERTY()
		bool followerAnimFinished = false;

	UPROPERTY()
		FString leaderAnimKey;

	UPROPERTY()
		FString followerAnimKey;

};
