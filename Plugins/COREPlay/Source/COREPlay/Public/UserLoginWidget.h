#pragma once

#include "CoreMinimal.h"
#include "ComplexUserWidget.h"
#include "CommonWidgets.h"
#include "UserProfile.h"
#include "UserManager.h"
#include "HTTPManager.h"
#include "JsonTools.h"
#include "UITools.h"

#include "Serialization/JsonSerializer.h"

#include "UserLoginWidget.generated.h"




UCLASS()
class COREPLAY_API UUserLoginWidget : public UComplexUserWidget {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<UComplexButton> profileWidgetClass = nullptr;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void init() override;

	UFUNCTION(BlueprintCallable)
		virtual void showProfileList(FString message);

	UFUNCTION(BlueprintCallable)
		virtual void showCreateUser(FString message);

	UFUNCTION(BlueprintCallable)
		virtual void showEnterCredentials(FString message);

	UFUNCTION(BlueprintCallable)
		virtual void showProcessing(FString message);

	UFUNCTION(BlueprintCallable)
		virtual void doRegister();
	virtual void applyCreateProfileOtherInputs(UUserProfile* profile);
	virtual void applyCreateProfileTransactionExtraData(UUserLoginTransaction* transaction, UUserProfile* profile);

	UFUNCTION(BlueprintCallable)
		virtual void doLogin();

	UFUNCTION()
		virtual void selectProfile(UUserWidget* child);

	UFUNCTION()
		virtual void onReceiveRegisterResponse(UHTTPTransaction* transaction, FString response);

	UFUNCTION()
		virtual void onReceiveLoginResponse(UHTTPTransaction* transaction, FString response);
};