#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CommonNavigationLinks.h"
#include "TargetDetail.h"
#include "XMLTools.h"

#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"

class ACreature;

#include "MindComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UMindComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		bool debugPathing = false;

	UPROPERTY()
		FTargetDetail target;

	UPROPERTY()
		float targetRefreshTimer = 0.0f;

	UPROPERTY()
		FRotator targetControlRotation;

	UPROPERTY()
		float delay = 1.0f;

	UPROPERTY()
		TArray< FNavPathNode > currentPath;

	UPROPERTY()
		float pathRefreshTimer = 0.0f;

	UPROPERTY()
		bool pathFindingDelegateBound = false;

	UPROPERTY()
		float jumpCooldown = 0.0f;

	UPROPERTY()
		UAIPerceptionComponent* perception = nullptr;

	UPROPERTY()
		UAISenseConfig_Sight* sight = nullptr;

	FNavPathQueryDelegate pathFindingCompletedDelegate;


	UMindComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	}

	virtual void init();
	virtual void initAIPerception();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void update(ACreature* creature, float deltaTime);

	virtual void findPath(ACreature* creature, FVector targetPosition, float deltaTime);
	void finishPathFinding(uint32 pathID, ENavigationQueryResult::Type resultType, FNavPathSharedPtr path);

	virtual FNavAgentProperties getNavProperties();

	virtual void tryJump(ACreature* creature);
	virtual bool moveTowards(float deltaTime, ACreature* creature, FVector location, float range = 0.0f);
	virtual void registerSeeActor(AActor* actor);

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

	static UMindComponent* get(AActor* actor);
	static UMindComponent* add(AActor* actor);
	static void remove(AActor* actor);
};