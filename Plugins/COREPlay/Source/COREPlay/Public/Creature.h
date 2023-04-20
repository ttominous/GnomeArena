#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#include "CreatureSpec.h"
#include "CreatureAnimInstance.h"
#include "Payload.h"
#include "DamageDetails.h"
#include "XMLTools.h"

#include "Creature.generated.h"

class UActionComponent;
class UTeam;

UCLASS()
class COREPLAY_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:

	static int lastID;
	static TArray< ACreature* > instances;

	static float replicationSmoothRate;


	UPROPERTY(replicated)
		int id = -1;

	UPROPERTY(EditAnywhere, replicated)
		bool ready = false;

	UPROPERTY(EditAnywhere)
		bool spawned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool dead = false;

	UPROPERTY(replicated)
		bool isBot = false;

	UPROPERTY()
		bool isRagdolling = false;

	UPROPERTY(EditAnywhere, replicated)
		FString name;

	UPROPERTY()
		FVector2D inputMove = FVector2D::ZeroVector;

	UPROPERTY()
		FRotator controlRotation = FRotator::ZeroRotator;

	UPROPERTY()
		FRotator netControlRotation = FRotator::ZeroRotator;

	UPROPERTY()
		float bodyYaw = 0.0f;

	UPROPERTY(EditAnywhere)
		bool aimLocked = false;

	UPROPERTY(replicated)
		float despawnTime;

	UPROPERTY()
		float inputReplicationTimer = 0.0f;

	UPROPERTY()
		bool playerInputPressed = false;

	UPROPERTY()
		FPayload clientToServerPayload;

	UPROPERTY()
		UActionComponent* actionComponent = nullptr;

	UPROPERTY(EditAnywhere, replicated)
		UCreatureSpec* spec = nullptr;

	UPROPERTY(replicated, ReplicatedUsing = OnRep_Team)
		UTeam* team = nullptr;

	UPROPERTY()
		bool hiddenFromOwner = false;

	ACreature();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	static ACreature* getByID(int id);


	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void showToOwner();
	virtual void hideFromOwner();

	virtual UActionComponent* getActionComponent();

	virtual float getCapsuleRadius();
	virtual float getCapsuleHalfHeight();
	virtual float getMoveRate();
	virtual float getMovePerc();
	virtual float getInputYaw();

	virtual void faceMovement(float deltaTime);
	virtual void faceControl();
	virtual void faceTarget(AActor* target, float deltaTime);
	virtual void faceYaw(float yaw, float deltaTime);
	virtual void snapToBodyYaw();
	virtual void snapMeshPosition();

	virtual UCreatureAnimInstance* getAnimInstance();
	virtual UAnimSequence* getAnimSequence(FString key);
	virtual UAnimMontage* playAnimSequence(FString key, float rate = 1.0f, float startTime = 0.0f, float blendIn = 0.05f, float blendOut = 0.2f, int loops = 1, FString slot = "DefaultSlot");
	virtual UAnimMontage* playAnimSequence(UAnimSequence* animSequence, float rate = 1.0f, float startTime = 0.0f, float blendIn = 0.05f, float blendOut = 0.2f, int loops = 1, FString slot = "DefaultSlot");
	virtual void fadeOutCurrentAnim(UAnimMontage* montage, float fadeOut, FString slot = "DefaultSlot");
	virtual void stopLoopingAnimSequence(UAnimMontage* montage);
	virtual void stopSlotAnimation(float fadeOut = 0.1f, FString slot = "DefaultSlot");
	virtual void handlePayload(FPayload payload);

	virtual void initID();
	virtual bool spawn();
	virtual void clearInput();
	virtual void processInput(float deltaTime);
	virtual void rebindInput();
	virtual void rebindInput(bool uiFocused);
	virtual void bindInput(UInputComponent* inputComponent, bool uiFocused);
	virtual void resetControlRotation();
	virtual void deSpawn();

	UFUNCTION(BlueprintCallable)
		virtual void kill();

	UFUNCTION(BlueprintCallable)
		virtual void processKill(FString killerName, FColor killerColor, FString iconKey, FString effectKey);


	virtual void detachMesh(bool updateAnimsImmediately);
	virtual void detachMesh(FVector location, FRotator rotation, bool updateAnimsImmediately);
	virtual void reattachMesh(bool updateAnimsImmediately);
	virtual void updateAnimationsImmediately();
	virtual void recenterMesh();

	virtual void lookAt(FVector location, bool turnHead);
	virtual void clearLookAt();

	virtual bool isFalling();
	virtual bool launch(FVector launchAmount);
	virtual bool tryJumpUp();
	virtual bool canStartJump();
	virtual bool startJump();
	virtual void startRagdoll();

	UFUNCTION(BlueprintCallable)
		virtual void refreshMaterials();

	virtual int getActualDamageAmount(FTargetDetail& attacker, FDamageDetail& damageDetail);
	virtual float getActualStagger(FDamageDetails& damage);
	virtual float getActualForce(FDamageDetails& damage);

	virtual void processServerDamage(FDamageDetails damage);
	virtual void processClientDamage(FDamageDetails damage, bool killingBlow);
	UFUNCTION(NetMulticast, Reliable)
		void sendDamage(FDamageDetails damage, bool killingBlow);
	virtual FString getHitReactionAnim(FDamageDetails& damage);

	virtual void PossessedBy(AController* NewController) override;
	UFUNCTION(Client, Reliable)
		virtual void onPossessed(AController* NewController);

	virtual void UnPossessed() override;
	UFUNCTION(Client, Reliable)
		virtual void onUnpossessed();

	virtual void updateInputPayload();
	virtual void sendInputToServer(float deltaTime);
	virtual void sendInputToClients(float deltaTime);

	UFUNCTION(Server, Reliable)
		void requestClientInput(FPayload payload);

	UFUNCTION(NetMulticast, Reliable)
		void sendClientInput(FPayload payload);

	virtual void receiveClientInput(FPayload payload);

	UFUNCTION(NetMulticast, Reliable)
		void sendKill(FDamageDetails damage);

	virtual void sendStagger(FDamageDetails damage);

	UFUNCTION(Server, Reliable)
		void requestPayload(FPayload payload);

	UFUNCTION(NetMulticast, Reliable)
		void sendPayload(FPayload payload);

	UFUNCTION()
	void OnRep_Team() {
		if (spawned) {
			refreshMaterials();
		}
	}

	UFUNCTION()
		virtual void pressInputMoveX(float value);

	UFUNCTION()
		virtual void pressInputMoveY(float value);

	UFUNCTION()
		virtual void pressJump();


	UFUNCTION(BlueprintCallable)
		virtual void equipPrimaryWeapon(FString itemKey);

	virtual void makeBot();
	virtual void makePlayerControlled();
	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);


	static ACreature* spawn(UWorld* world, FString specKey, FVector position, float yaw);
	static ACreature* spawn(UWorld* world, UCreatureSpec* spec, FVector position, float yaw);
	static ACreature* spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw);
	static void destroyAll();
};
