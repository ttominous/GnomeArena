#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "XMLTools.h"

#include "SceneObject.generated.h"

class USceneObjectSpec;
class UVesselComponent;

UCLASS()
class COREPLAY_API ASceneObject : public AActor {
	GENERATED_BODY()

public:

	static int lastID;
	static TArray< ASceneObject* > instances;

	UPROPERTY()
		bool isGhost = false;

	UPROPERTY(replicated)
		int id = -1;

	UPROPERTY()
		bool initialized = false;

	UPROPERTY(EditAnywhere)
		UBoxComponent* collisionComponent;

	UPROPERTY(replicated)
		USceneObjectSpec* spec = nullptr;

	UPROPERTY()
		UVesselComponent* vesselComponent = nullptr;

	static ASceneObject* getByID(int id);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	ASceneObject();

	virtual void init();
	virtual void build();
	virtual void makeGhost();
	virtual void updateGhostMaterial(bool validPlacement);
	virtual bool isAcceptibleOverlap(FOverlapInfo overlap, bool considerCreatures);
	virtual bool isAcceptibleOverlap(FOverlapResult overlap, bool considerCreatures);
	virtual bool isAcceptibleOverlap(AActor* actor, USceneComponent* component, FVector location, int itemIndex);
	virtual bool isOverlappingUnacceptible();
	virtual bool isMovable();

	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

	static ASceneObject* spawn(UWorld* world, FString specKey, FVector position, float yaw, bool asGhost);
	static ASceneObject* spawn(UWorld* world, USceneObjectSpec* spec, FVector position, float yaw, bool asGhost);
	static ASceneObject* spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw, bool asGhost);
	static void destroyAll();
};