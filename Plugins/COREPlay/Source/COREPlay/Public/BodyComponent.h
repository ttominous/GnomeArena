#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"

#include "BodyPart.h"
#include "XMLTools.h"
#include "Payload.h"

#include "BodyComponent.generated.h"

class UVoiceOverComponent;

USTRUCT(BlueprintType)
struct COREPLAY_API FBodyPartSlot {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString bodyPartKey;

};

USTRUCT(BlueprintType)
struct COREPLAY_API FBodyShape {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		float value;

};

USTRUCT(BlueprintType)
struct COREPLAY_API FBodyScalarParameter {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		float value;

};

USTRUCT(BlueprintType)
struct COREPLAY_API FBodyVectorParameter {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FVector value;

};

USTRUCT(BlueprintType)
struct COREPLAY_API FBodyTextureParameter {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString key;

	UPROPERTY(EditAnywhere)
		FString value;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UBodyComponent : public UActorComponent {
	GENERATED_BODY()

public:

	static TArray< UBodyComponent* > instances;

	UPROPERTY(replicated, EditAnywhere)
		FString rootSlotKey = "Body";

	UPROPERTY(replicated, EditAnywhere)
		TSubclassOf< UAnimInstance > animInstanceClass = nullptr;

	UPROPERTY(EditAnywhere)
		UClass* currentInstanceClass = nullptr;

	UPROPERTY()
		UAnimInstance* animInstance = nullptr;

	UPROPERTY(replicated, EditAnywhere, ReplicatedUsing = OnRep_RebuildAll)
		TArray< FBodyPartSlot > slots;

	UPROPERTY(replicated, EditAnywhere)
		bool armed = true;

	UPROPERTY()
		FName rootBone = "Hips";

	UPROPERTY()
		float armedCooldown = 0.0f;

	UPROPERTY()
		bool dirty = true;

	UPROPERTY()
		bool dirtyShapes = true;

	UPROPERTY()
		bool dirtyMaterialParameters = true;

	UPROPERTY()
		float dirtyTimer;

	UPROPERTY()
		TMap< FString, USceneComponent* > components;

	UPROPERTY()
		USceneComponent* rootComponent = nullptr;

	UPROPERTY(EditAnywhere, replicated, ReplicatedUsing = OnRep_RebuildMaterialParameters)
		TArray< FBodyScalarParameter > globalScalarParameters;

	UPROPERTY(EditAnywhere, replicated, ReplicatedUsing = OnRep_RebuildMaterialParameters)
		TArray< FBodyVectorParameter > globalVectorParameters;

	UPROPERTY(EditAnywhere, replicated, ReplicatedUsing = OnRep_RebuildMaterialParameters)
		TArray< FBodyTextureParameter > globalTextureParameters;

	UPROPERTY(EditAnywhere)
		TArray< FBodyScalarParameter > localScalarParameters;

	UPROPERTY(EditAnywhere)
		TArray< FBodyVectorParameter > localVectorParameters;

	UPROPERTY(EditAnywhere)
		TArray< FBodyTextureParameter > localTextureParameters;

	UPROPERTY(EditAnywhere)
		TMap< FString, float > morphs;

	UPROPERTY(replicated, EditAnywhere, ReplicatedUsing = OnRep_RebuildShapes)
		TArray< FBodyShape > shapes;

	UPROPERTY()
		float bodyScaleSpeed = 1.0f;

	UBodyComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UBodyComponent, rootSlotKey);
		DOREPLIFETIME(UBodyComponent, animInstanceClass);
		DOREPLIFETIME(UBodyComponent, slots);
		DOREPLIFETIME(UBodyComponent, armed);
		DOREPLIFETIME(UBodyComponent, shapes);
		DOREPLIFETIME(UBodyComponent, globalScalarParameters);
		DOREPLIFETIME(UBodyComponent, globalVectorParameters);
		DOREPLIFETIME(UBodyComponent, globalTextureParameters);
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void clear();
	virtual void clearSlots();
	virtual void clearParameters();
	virtual void clearShapes();
	virtual void destroy();
	virtual void rebuild();
	virtual void clearAndBuild();
	virtual void rebuildAnimInstance(UClass* targetClass);
	virtual void rebuildAnimInstance();
	virtual void rebuildShapes();
	virtual void destroySlot(FBodyPartSlot& slot);
	virtual UBodyPart* getBodyPart(FString slotKey);
	virtual UBodyPart* getBodyPart(FBodyPartSlot& slot);
	virtual FString getFinalAttachSlot(FBodyPartSlot& slot, FBodyPartDetail& details, UBodyPart* bodyPart);
	virtual void buildSlot(FBodyPartSlot& slot);
	virtual USceneComponent* getRootComponent();
	virtual void applyBaseMaterials(FBodyPartDetail& details, UMeshComponent* meshComponent);
	virtual void applyMaterials(FBodyPartDetail& details, UMeshComponent* meshComponent, bool includeTextures);
	virtual void applyMaterialParameter(UMaterialInstanceDynamic* material, FString parameterKey, FString finalKey, bool includeTextures);

	virtual void applyBaseMorphs(FBodyPartDetail& details, USkeletalMeshComponent* meshComponent);
	virtual void applyMorphsAndShapes(FBodyPartDetail& details, USkeletalMeshComponent* meshComponent);
	virtual void reapplyMorphsAndShapes();
	virtual void reapplyMaterials();

	virtual void refreshHiddenFromOwner();

	virtual void arm();
	virtual void unarm();
	static void arm(AActor* actor);
	static void unarm(AActor* actor);

	virtual void setSlot(FString key, FString value);
	virtual FString getSlotValue(FString key);
	virtual void setMorph(FString key, float value, bool applyImmediately = false);
	virtual float getMorph(FString key);
	virtual void setShape(FString key, float value);
	virtual float getShape(FString key);

	UFUNCTION(BlueprintCallable)
	virtual void setScalarParameter(FString key, double value, bool global);

	UFUNCTION(BlueprintCallable)
	virtual double getScalarParameter(FString key);

	virtual double getFinalScalarParameter(FString key, double currentValue);

	UFUNCTION(BlueprintCallable)
	virtual void setVectorParameter(FString key, FVector value, bool global);

	UFUNCTION(BlueprintCallable)
	virtual FVector getVectorParameter(FString key);

	virtual FVector getFinalVectorParameter(FString key, FVector currentValue);

	UFUNCTION(BlueprintCallable)
	virtual void setColorParameter(FString key, FColor value, bool global);

	UFUNCTION(BlueprintCallable)
	virtual FColor getColorParameter(FString key);

	virtual FColor getFinalColorParameter(FString key, FColor currentValue);

	UFUNCTION(BlueprintCallable)
	virtual void setTextureParameter(FString key, FString value, bool global);

	UFUNCTION(BlueprintCallable)
	virtual FString getTextureParameter(FString key);
	virtual FString getFinalTextureParameter(FString key, FString currentValue);

	virtual void processPayload(FPayload payload);

	virtual void setSimulatePhysics(FString boneName, float amount, bool includeSelf);
	virtual void applyDefaultPhysics(USceneComponent* sceneComponent, FBodyPartDetail& details);
	virtual void applyRootDefaultPhysics();

	virtual void copyTo(UBodyComponent* otherBody);

	virtual USceneComponent* getComponent(FString slotKey);


	UFUNCTION()
		void OnRep_RebuildAll();

	UFUNCTION()
		void OnRep_RebuildShapes();

	UFUNCTION()
		void OnRep_RebuildMaterialParameters();


	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

	UFUNCTION(BlueprintCallable)
	static UBodyComponent* get(AActor* actor);
	static void rebuildAll();
};
