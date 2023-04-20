#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "RagdollComponent.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FRagdollBoneTransform {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString boneName;

	UPROPERTY(EditAnywhere)
		FIntVector location;

	UPROPERTY(EditAnywhere)
		FIntVector rotation;

	FTransform getTransform();
	void set(FTransform& transform);
	static FRagdollBoneTransform fromTransform(FTransform& transform);

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API URagdollComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY()
		bool initialized = false;

	UPROPERTY()
		FString rootBone = "Hips";

	UPROPERTY()
		USkeletalMeshComponent* meshComponent = nullptr;

	UPROPERTY()
		float updateTimer = 0.0f;

	UPROPERTY()
		float highFrequencyUpdateTimer = 1.0f;

	UPROPERTY()
		TArray< FRagdollBoneTransform > boneTransforms;

	UPROPERTY()
		UClass* originalAnimInstanceClass = nullptr;

	URagdollComponent();

	/*virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UBodyComponent, rootSlotKey);
	}*/

	virtual void init();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(NetMulticast, Unreliable)
		void sendBoneTransforms(const TArray< FRagdollBoneTransform >& inBoneTransforms);
	virtual void applyBoneTransforms(const TArray< FRagdollBoneTransform >& inBoneTransforms);

	static URagdollComponent* get(AActor* actor);
	static URagdollComponent* startRagdolling(AActor* actor);
	static void stopRagdolling(AActor* actor);
	static void expediteReplicate(AActor* actor);

};