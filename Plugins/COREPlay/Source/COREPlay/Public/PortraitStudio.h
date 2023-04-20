#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/LocalLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "PortraitStudio.generated.h"

class UBodyComponent;

UCLASS()
class COREPLAY_API UPortrait : public UObject {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool built = false;

	UPROPERTY()
		float waitTime = 0.0f;

	UPROPERTY()
		FString key;

	UPROPERTY()
		UObject* target = nullptr;

	UPROPERTY()
		UMaterialInstanceDynamic* material = nullptr;

};

UCLASS()
class COREPLAY_API UPortraitBody : public UObject {
	GENERATED_BODY()

public:

	virtual void clear();

};

UCLASS()
class COREPLAY_API UPortraitLivingBody : public UPortraitBody {
	GENERATED_BODY()

public:

	UPROPERTY()
		UBodyComponent* bodyComponent = nullptr;

	virtual void clear() override;

};

UCLASS()
class COREPLAY_API UPortraitStaticBody : public UPortraitBody {
	GENERATED_BODY()

public:

	UPROPERTY()
		TMap< FString, USceneComponent* > components;

	virtual void clear() override;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UPortraitStudioSceneCaptureComponent2D : public USceneCaptureComponent2D {
	GENERATED_BODY()

public:

	virtual const AActor* GetViewOwner() const override { return GetOwner(); }

};

UCLASS()
class COREPLAY_API APortraitStudio : public AActor {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* physicsCapsule = nullptr;

	UPROPERTY(EditAnywhere)
		UPortraitStudioSceneCaptureComponent2D* captureComponent;

	UPROPERTY()
		bool working = false;

	UPROPERTY()
		UPortraitBody* body = nullptr;

	UPROPERTY()
		TArray< UPortrait* > orders;

	UPROPERTY()
		FTransform transform;

	UPROPERTY()
		USpringArmComponent* cameraArm = nullptr;

	UPROPERTY()
		ULocalLightComponent* light = nullptr;

	APortraitStudio();
	virtual void init();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void addOrder(UPortrait* order);
	virtual void executeNextWorkOrder(float deltaTime);
	virtual void buildFromCreature();
	virtual void buildFromSceneObject();

	virtual USpringArmComponent* getCameraArm();
	virtual ULocalLightComponent* getLight();
	virtual void scaleLight(float range);
};
