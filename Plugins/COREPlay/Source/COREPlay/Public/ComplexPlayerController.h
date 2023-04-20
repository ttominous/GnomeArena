#pragma once

#include "CoreMinimal.h"
#include "SaveManager.h"
#include "HUDLayer.h"

#include "GameFramework/PlayerController.h"

#include "ComplexPlayerController.generated.h"

UCLASS()
class COREPLAY_API AComplexPlayerController : public APlayerController {

	GENERATED_BODY()

public:

	UPROPERTY()
		bool paused = false;

	UPROPERTY()
		bool usingGamepad = false;

	UPROPERTY()
		bool uiFocused = false;

	UPROPERTY()
		bool gameShowCursor = false;

	UPROPERTY()
		bool verifiedWantsGamepad = true;

protected:

	virtual void SetupInputComponent() override;

public:

	UFUNCTION()
		void togglePause();

	virtual void pause();
	virtual void unpause();
	virtual void quit();

	virtual void rebindInputs();
	virtual void bindUIInputs();
	virtual void rebindPawnInputs();

	virtual void updateShowCursor();

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
		static void pauseController(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		static void unpauseController(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		static void quitController(APlayerController* controller);

	UFUNCTION(Client, Reliable)
		void requestProfileInfo();

	UFUNCTION(Server, Reliable)
		void receiveProfileInfo(const FString& profileName);

	UFUNCTION(Server, Reliable)
		void sendLeaveNotification();

};