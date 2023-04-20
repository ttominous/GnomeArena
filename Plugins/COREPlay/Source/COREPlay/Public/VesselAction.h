#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "ActionInstance.h"
#include "PlayAnimationData.h"
#include "VesselComponent.h"

#include "VesselAction.generated.h"

UCLASS()
class COREPLAY_API UVesselAction : public UAction {

	GENERATED_BODY()

public:

	virtual void processDeferredStart(UActionInstance* instance, UActionComponent* component) override;
	virtual void processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) override;
	virtual bool processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) override;
	virtual void processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload);
	virtual void processAnimationFinished(UActionInstance* instance, UActionComponent* component);
	virtual void processInterupt(UActionInstance* instance, UActionComponent* component, FVector targetPosition) override;

	virtual UClass* getInstanceClass(UActionComponent* component) override;

	virtual void snapToVesselSlot(UActionInstance* instance, UActionComponent* component);
	virtual void startLoopingAnimation(UActionInstance* instance, UActionComponent* component);

	virtual void writeXML(FXMLBuilderNode& rootNode, UActionInstance* instance, UActionComponent* component) override;
	virtual void readXML(FXmlNode* rootNode, UActionInstance* instance, UActionComponent* component) override;

};

UCLASS(Blueprintable)
class COREPLAY_API UVesselActionInstance : public UActionInstance {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		bool initComplete = false;

	UPROPERTY(EditAnywhere)
		bool entryComplete = false;

	UPROPERTY(EditAnywhere)
		bool exiting = false;

	UPROPERTY()
		UVesselComponent* vessel = nullptr;

	UPROPERTY()
		FString slot;

	UPROPERTY()
		int accessIndex;

	UPROPERTY()
		FString animSequenceKey;

	UPROPERTY()
		float targetYaw;

	UPROPERTY()
		FVector targetLocation;

};
