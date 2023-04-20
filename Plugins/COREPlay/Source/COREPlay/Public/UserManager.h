#pragma once

#include "CoreMinimal.h"
#include "UserProfile.h"
#include "HTTPManager.h"
#include "JsonTools.h"

#include "GameFramework/PlayerController.h"

#include "UserManager.generated.h"

UCLASS()
class COREPLAY_API UUserLoginTransaction : public UHTTPTransaction {

	GENERATED_BODY()

public:

	UPROPERTY()
		UUserProfile* profile;

	TSharedPtr<FJsonObject> extraData;

	UUserLoginTransaction();
};



UCLASS()
class COREPLAY_API UUserManager : public UObject {

	GENERATED_BODY()

public:

	static UUserManager* instance;

	UPROPERTY()
		bool loaded = false;

	UPROPERTY()
		float pingTime = 2.0f;

	UPROPERTY()
		float pingFrequency = 3.0f;

	UPROPERTY()
		TArray< UUserProfile* > possibleProfiles;

	UPROPERTY()
		TMap< int, UUserProfile* > loadedProfiles;

	UPROPERTY()
		FString lastProfileKey;

	UPROPERTY()
		FString lastTestProfileKey;

	UPROPERTY()
		bool autoLoggingIn = false;

	virtual void update(float deltaTime);
	virtual void loadProfiles();
	virtual void saveProfiles();
	virtual UClass* getProfileClass();

	virtual UUserProfile* getProfile(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		static UUserProfile* getProfileStatic(APlayerController* controller);


	virtual bool profileExistsLocally(UUserProfile* profile);
	virtual void registerNewProfile(APlayerController* controller, UUserProfile* profile);
	virtual void registerExistingProfile(APlayerController* controller, UUserProfile* profile);

	UFUNCTION()
		virtual void onReceiveLoginSuccess(UHTTPTransaction* transaction, FString response);

	UFUNCTION()
		virtual void onReceiveLoginFailure(UHTTPTransaction* transaction, FString response);

	virtual void ping(float deltaTime, FString activityString, FString matchName, FString levelName, FString matchStatus, int matchPlayerCount, int matchPlayerMax);

	UFUNCTION()
		virtual void onPingResponse(UHTTPTransaction* transaction, FString response);
};