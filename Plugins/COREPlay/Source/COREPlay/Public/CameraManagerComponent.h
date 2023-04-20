#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"

#include "CameraManagerComponent.generated.h"


UENUM(BlueprintType)
enum class ECameraMode : uint8 {
	NONE = 0							UMETA(DisplayName = "None"),
	THIRD_PERSON_FREE = 1				UMETA(DisplayName = "ThirdPersonFree"),
	THIRD_PERSON_SHOOTER = 2			UMETA(DisplayName = "ThirdPersonShooter"),
	TOP_DOWN = 3						UMETA(DisplayName = "TopDown"),
	CHARACTER_FACE_LIMITED = 4			UMETA(DisplayName = "CharacterFaceLimited"),
	CHARACTER_BODY_LIMITED = 5			UMETA(DisplayName = "CharacterBodyLimited"),
	CHARACTER_DEAD = 6					UMETA(DisplayName = "CharacterDead"),
	RTS = 7								UMETA(DisplayName = "RTS")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UCameraManagerComponent : public UActorComponent {
	GENERATED_BODY()

public:

	static TArray< UCameraManagerComponent* > instances;

	UPROPERTY()
		bool spawned = false;

	UPROPERTY()
		USpringArmComponent* cameraArm = nullptr;

	UPROPERTY()
		UCameraComponent* camera = nullptr;

	UPROPERTY()
		float targetArmLength = 600.0f;

	UPROPERTY()
		float minTargetArmLength = 0.0f;

	UPROPERTY()
		float maxTargetArmLength = 0.0f;

	UPROPERTY()
		float targetArmLengthZoomStep = 0.0f;

	UPROPERTY()
		float targetYaw = 0.0f;

	UPROPERTY()
		float targetPitch = 0.0f;

	UPROPERTY()
		float turnSpeed = 1.0f;

	UPROPERTY()
		bool canTurnYaw = true;

	UPROPERTY()
		bool canTurnPitch = true;

	UPROPERTY()
		bool canZoom = true;

	UPROPERTY()
		bool collisionEnabled = true;

	UPROPERTY()
		bool showCursor = false;

	UPROPERTY()
		bool needsSyncOwner = true;

	UPROPERTY(EditAnywhere)
		ECameraMode mode = ECameraMode::THIRD_PERSON_FREE;

	UPROPERTY(EditAnywhere)
		ECameraMode defaultMode = ECameraMode::THIRD_PERSON_FREE;

	UPROPERTY(EditAnywhere)
		float defaultArmLength = 350.0f;

	UPROPERTY(EditAnywhere)
		FString defaultBoneName = "Neck";

	UPROPERTY()
		APlayerController* controller = nullptr;

	UPROPERTY()
		AActor* focusedOnActor = nullptr;

	UPROPERTY()
		bool snapToPointEnabled = false;

	UPROPERTY()
		FVector snapToPointLocation;

	UPROPERTY()
		bool draggingRotation = false;

	UPROPERTY()
		FVector lastLocation;

	UPROPERTY()
		FRotator lastRotation;

	UPROPERTY()
		float lastTargetArmLength;

	UPROPERTY()
		FVector lastOffset;

	UPROPERTY()
		bool justSwitchedModes = false;

	UPROPERTY()
		AActor* aimAssistTarget = nullptr;

	UPROPERTY()
		float aimAssistWeight = 0.0f;

	UPROPERTY()
		float aimAssistWeightXPositiveTurn = 0.0f;

	UPROPERTY()
		float aimAssistWeightXNegativeTurn = 0.0f;

	UPROPERTY()
		float aimAssistWeightYPositiveTurn = 0.0f;

	UPROPERTY()
		float aimAssistWeightYNegativeTurn = 0.0f;


	UCameraManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual void spawn();
	virtual void initCameraArm();
	virtual void initCamera();
	virtual void switchMode(ECameraMode newMode);
	virtual void setDefaultMode(ECameraMode newMode);
	virtual void switchDefaultMode();
	virtual void setCollisionEnabled(bool enabled);
	virtual void setShowCursor(bool inShowCursor);
	virtual void update(float deltaTime);
	virtual void updateController();
	virtual void updateThirdPersonFreeCamera(float deltaTime);
	virtual void updateThirdPersonShooterCamera(float deltaTime);
	virtual void updateTopDownCamera(float deltaTime);
	virtual void updateCharacterLimitedCamera(float deltaTime);
	virtual void updateCharacterDeadCamera(float deltaTime);
	virtual void updateRTSCamera(float deltaTime);

	virtual void bindInput(bool uiFocused);
	virtual void clearInput();
	virtual void handleTurnYaw(float amount);
	virtual void handleTurnPitch(float amount);
	virtual void handleTurn(FVector2D amount);
	virtual void syncControlRotation();
	virtual bool isBot();

	UFUNCTION()
		virtual void pressDragRotation();

	UFUNCTION()
		virtual void releaseDragRotation();

	UFUNCTION()
		virtual void pressZoomIn();

	UFUNCTION()
		virtual void pressZoomOut();

	virtual FVector getCameraLocation();
	virtual FRotator getCameraRotation();
	virtual void snapToPoint(FVector location);
	virtual void zoomIn();
	virtual void zoomOut();

	static UCameraManagerComponent* get(AActor* actor);
	static void remove(AActor* actor);
	static UCameraManagerComponent* add(AActor* actor);
};
