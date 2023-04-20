#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature.h"

#include "DualAxisCreature.generated.h"

UCLASS()
class COREPLAY_API ADualAxisCreature : public ACreature
{
	GENERATED_BODY()

public:

	UPROPERTY()
		bool sprinting = false;

	UPROPERTY()
		bool aiming = false;

	UPROPERTY()
		bool primaryAttack = false;

	UPROPERTY()
		bool secondaryAttack = false;

	UPROPERTY(replicated, ReplicatedUsing = OnRep_AltHanded)
		bool altHanded = false;

	UPROPERTY()
		FVector aimAtPoint;

	UPROPERTY()
		FVector netAimAtPoint;

	UPROPERTY(replicated)
		bool crouched = false;

public:

	virtual float getMoveRate() override;
	virtual bool isTryingToSprint();
	virtual bool isSprinting();
	virtual bool isAiming();
	virtual void updateInputPayload() override;
	virtual bool spawn() override;
	virtual void processInput(float deltaTime) override;
	virtual void bindInput(UInputComponent* inputComponent, bool uiFocused) override;
	virtual bool startJump() override;

	virtual void handlePayload(FPayload payload) override;

	virtual void receiveClientInput(FPayload payload) override;

	virtual void attachWeapons();
	virtual FVector getWeaponTip();
	virtual USceneComponent* getWeaponComponent();
	virtual float getAimAccuracy();

	virtual void updateAimAtPoint(float deltaTime);

	virtual FString getHitReactionAnim(FDamageDetails& damage) override;

	UFUNCTION()
		virtual void pressInputSprint();

	UFUNCTION()
		void releaseInputSprint();

	UFUNCTION()
		virtual void pressInputAim();

	UFUNCTION()
		virtual void releaseInputAim();

	UFUNCTION()
		virtual void pressInputPrimaryAttack();

	UFUNCTION()
		virtual void releaseInputPrimaryAttack();

	UFUNCTION()
		virtual void pressInputSecondaryAttack();

	UFUNCTION()
		virtual void releaseInputSecondaryAttack();

	UFUNCTION()
		virtual void pressInputSwitchHands();

	UFUNCTION()
		virtual void pressInputCrouch();

	UFUNCTION()
		virtual void pressInputReload();

	UFUNCTION()
	void OnRep_AltHanded() {
		attachWeapons();
	}
};
