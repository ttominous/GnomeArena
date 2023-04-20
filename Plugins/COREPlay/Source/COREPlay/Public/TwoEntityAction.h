#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionInstance.h"

#include "TwoEntityAction.generated.h"

class UActionInstance;
class UActionComponent;

UCLASS()
class COREPLAY_API UTwoEntityAction : public UAction {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		bool faceLeader = true;

	UPROPERTY(EditAnywhere)
		bool snapToLeader = false;

	UPROPERTY(EditAnywhere)
		bool faceFollower = true;

	UPROPERTY(EditAnywhere)
		bool snapToFollower = false;

	virtual bool processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) override;
	virtual void processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) override;
	virtual void processEnd(UActionInstance* instance, UActionComponent* component) override;
	virtual void processActualEnd(UActionInstance* instance, UActionComponent* component);

	virtual void processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) override;
	virtual void processUpdate(float deltaTime, UActionInstance* instance, UActionComponent* component);

	virtual void processLeaderAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload);
	virtual void processFollowerAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload);

	virtual void processLeaderAnimationFinished(UActionInstance* instance, UActionComponent* component);
	virtual void processFollowerAnimationFinished(UActionInstance* instance, UActionComponent* component);

	virtual bool positionLeader(UActionInstance* instance, UActionComponent* component, float deltaTime);
	virtual bool positionFollower(UActionInstance* instance, UActionComponent* component, float deltaTime);

	virtual bool isLeader(UActionInstance* instance, UActionComponent* component);
	virtual UClass* getInstanceClass(UActionComponent* component) override;

};




UCLASS(Blueprintable)
class COREPLAY_API UTwoEntityActionInstance : public UActionInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool started = false;

	UPROPERTY()
		bool initialPositioned = false;

	UPROPERTY()
		bool ended = false;

	UPROPERTY()
		AActor* leader = nullptr;;

	UPROPERTY()
		UActionComponent* leaderActionComponent = nullptr;;

	UPROPERTY()
		FString leaderAnimSequence;

	UPROPERTY()
		AActor* follower = nullptr;;

	UPROPERTY()
		FString followerAnimSequence;

	UPROPERTY()
		UActionComponent* followerActionComponent = nullptr;;

};