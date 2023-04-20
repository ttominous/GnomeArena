#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TargetDetail.h"

#include "FaceComponent.generated.h"

class UExpressionSet;
class UCreatureAnimInstance;


USTRUCT()
struct COREPLAY_API FExpressionInstance {

	GENERATED_BODY()

public:

	UPROPERTY()
		FString key;

	UPROPERTY()
		float amount;

	UPROPERTY()
		float time = 0.0f;

	UPROPERTY()
		float duration;

	UPROPERTY()
		float blendInEnd = 0.0f;

	UPROPERTY()
		float blendOutStart = 0.0f;

	UPROPERTY()
		float blendOutDuration = 0.0f;

};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UFaceComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		UExpressionSet* expressionSet = nullptr;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > morphs;

	UPROPERTY(EditAnywhere)
		TMap< FString, FExpressionInstance > expressions;

	UPROPERTY()
		USkeletalMeshComponent* meshComponent = nullptr;

	UPROPERTY(EditAnywhere)
		bool blinkEnabled = true;

	UPROPERTY()
		float nextBlink = 0.0f;

	UPROPERTY(EditAnywhere)
		float eyeWander = 0.25f;

	UPROPERTY()
		float nextEyeWander = 0.0f;

	UPROPERTY(EditAnywhere)
		FVector2D eyeWanderAmounts;

	UPROPERTY(EditAnywhere)
		bool microExpressionsEnabled = true;

	UPROPERTY()
		float nextMicroExpression = 0.0f;

	UPROPERTY()
		FTargetDetail eyeTarget;

	UPROPERTY(EditAnywhere)
		int maxMeshLOD = 1;

	UPROPERTY()
		bool active = true;

	UFaceComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual USkeletalMeshComponent* getMeshComponent();
	virtual UCreatureAnimInstance* getAnimInstance();
	virtual void addExpression(FString key, float amount, float duration, float blendInPerc, float blendOutPerc, bool replace);
	virtual void clearExpressions();
	virtual void updateExpressions(float deltaTime);

	virtual void setEyeTarget(UObject* object, FVector location);
	virtual void clearEyeTarget();
	virtual FVector2D getEyeTargetAmounts();
	virtual FTransform getHeadTransform();
	virtual FVector getHeadLocation();
	virtual void setEyeRotation(float x, float y);
	virtual void clearEyeRotation();

	virtual TArray< FString > getPossibleExpressionMorphs();

	virtual void applyPainExpression();
	virtual void applyDeadExpression();
	virtual void stopNaturalExpressions();
	virtual void applyNaturalExpressions();

	static UFaceComponent* get(AActor* actor);

};