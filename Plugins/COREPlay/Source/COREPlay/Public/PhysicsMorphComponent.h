#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "PhysicsMorphComponent.generated.h"

class UBodyComponent;

USTRUCT(BlueprintType)
struct FPhysicsMorphImpulse {

	GENERATED_BODY()

public:

	UPROPERTY()
		FVector impulse = FVector::ZeroVector;

	UPROPERTY()
		float degradeRate = 1.0f;

	UPROPERTY()
		bool localSpace;

};


USTRUCT(BlueprintType)
struct FPhysicsMorphSlot {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FName socketName;

	UPROPERTY(EditAnywhere)
		float gravityPerc = 1.0f;

	UPROPERTY(EditAnywhere)
		float motionPerc = 1.0f;

	UPROPERTY(EditAnywhere)
		float lerpRate = 100.0f;

	UPROPERTY(EditAnywhere)
		float distance = 2.0f;

	UPROPERTY(EditAnywhere)
		bool drawDebug = false;

	UPROPERTY()
		FVector lastPos;

	UPROPERTY()
		float forward = 0.0f;

	UPROPERTY()
		float left = 0.0f;

	UPROPERTY()
		float up = 0.0f;

	UPROPERTY(EditAnywhere)
		FName upMorphName = "Up";

	UPROPERTY(EditAnywhere)
		FName downMorphName = "Down";

	UPROPERTY(EditAnywhere)
		FName leftMorphName = "Left";

	UPROPERTY(EditAnywhere)
		FName rightMorphName = "Right";

	UPROPERTY(EditAnywhere)
		FName forwardMorphName = "Forward";

	UPROPERTY(EditAnywhere)
		FName backMorphName = "Back";

	UPROPERTY()
		TArray< FPhysicsMorphImpulse > impulses;

	UPROPERTY()
		TArray< USkeletalMeshComponent* > meshes;

};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UPhysicsMorphComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TMap< FString, FPhysicsMorphSlot > slots;

	UPhysicsMorphComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void rebuild(UBodyComponent* body);

	virtual void addImpulse(FString slotKey, FVector amount, float degradeRate, bool localSpace);

	static UPhysicsMorphComponent* get(AActor* actor);

};