#include "UserLoginWidget.h"
#include "UserManager.h"
#include "UserManager.h"
#include "ComplexPlayerController.h"
#include "ComplexHUD.h"
#include "ControllerTools.h"
#include "HTTPTools.h"
#include "GameTools.h"
#include "Debug.h"


void UUserLoginWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUserLoginWidget::init() {
	Super::init();
	showProfileList("");
}


void UUserLoginWidget::showProfileList(FString message) {

	if (message.Len() > 0) {
		UUITools::setTextBlockText(this, "ProfileList_Message", message);
		UUITools::setVisibility(this, "ProfileList_Message", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "ProfileList_Message", ESlateVisibility::Collapsed);
	}

	int localPlayerIndex = UControllerTools::getLocalPlayerIndex(GetOwningPlayer());

	TArray< UUserProfile* > unusedProfiles;
	for (UUserProfile* profile : UUserManager::instance->possibleProfiles) {
		bool valid = true;
		for (auto& usedProfile : UUserManager::instance->loadedProfiles) {
			if (localPlayerIndex != usedProfile.Key && usedProfile.Value == profile) {
				valid = false;
				break;
			}
		}
		if (valid) {
			unusedProfiles.Add(profile);
		}
	}

	UVerticalBox* profileContainer = UUITools::getVerticalBox(this, "ProfileContainer");
	if (!profileContainer) {
		return;
	}

	UUserWidget* createButton = UUITools::getUserWidget(this, "ProfileList_Create");
	if (createButton == nullptr) {
		return;
	}
	UUserWidget* loginButton = UUITools::getUserWidget(this, "ProfileList_Login");
	if (loginButton == nullptr) {
		return;
	}
	defaultGamePadFocus = createButton;

	UUITools::setNavigationVertical(createButton, loginButton);

	if (profileWidgetClass) {
		UUITools::syncVerticalBoxChildren(profileContainer, unusedProfiles.Num(), profileWidgetClass);
		TArray< UWidget* > 	profileEntries = profileContainer->GetAllChildren();
		for (int i = 0; i < profileEntries.Num(); i++) {
			UComplexButton* profileButton = Cast< UComplexButton >(profileEntries[i]);
			if (profileButton) {
				UUITools::setVisibility(profileButton, "OnlineProfile", unusedProfiles[i]->local ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

				profileButton->displayText = unusedProfiles[i]->username;
				profileButton->styleDirty = true;
				if (!profileButton->onSelectDelegate.IsBound()) {
					profileButton->onSelectDelegate.AddDynamic(this, &UUserLoginWidget::selectProfile);
				}

				if (i == 0) {
					defaultGamePadFocus = profileButton;
					UUITools::setNavigationVertical(loginButton, profileButton);
				}

				if (i == profileEntries.Num() - 1) {
					UUITools::setNavigationVertical(profileButton, createButton);
				}
				else {
					UComplexButton* nextButton = Cast< UComplexButton >(profileEntries[i+1]);
					if (nextButton) {
						UUITools::setNavigationVertical(profileButton, nextButton);
					}
				}
			}
		}

	}

	UUITools::setVisibility(this, "ProfileList", ESlateVisibility::Visible);
	UUITools::setVisibility(this, "CreateNew", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "EnterCredentials", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "Processing", ESlateVisibility::Hidden);

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}
}

void UUserLoginWidget::showCreateUser(FString message) {

	if (message.Len() > 0) {
		UUITools::setTextBlockText(this, "CreateNew_Message", message);
		UUITools::setVisibility(this, "CreateNew_Message", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "CreateNew_Message", ESlateVisibility::Collapsed);
	}

	UUITools::setVisibility(this, "ProfileList", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "CreateNew", ESlateVisibility::Visible);
	UUITools::setVisibility(this, "EnterCredentials", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "Processing", ESlateVisibility::Hidden);

	UUserWidget* nameField = UUITools::getUserWidget(this, "CreateNew_Name");
	if (nameField == nullptr) {
		return;
	}
	defaultGamePadFocus = nameField;

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}
}

void UUserLoginWidget::showEnterCredentials(FString message) {

	if (message.Len() > 0) {
		UUITools::setTextBlockText(this, "EnterCredentials_Message", message);
		UUITools::setVisibility(this, "EnterCredentials_Message", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "EnterCredentials_Message", ESlateVisibility::Collapsed);
	}

	UUITools::setVisibility(this, "ProfileList", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "CreateNew", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "EnterCredentials", ESlateVisibility::Visible);
	UUITools::setVisibility(this, "Processing", ESlateVisibility::Hidden);

	UUserWidget* nameField = UUITools::getUserWidget(this, "EnterCredentials_Name");
	if (nameField == nullptr) {
		return;
	}
	defaultGamePadFocus = nameField;

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(defaultGamePadFocus);
	}
}

void UUserLoginWidget::showProcessing(FString message) {

	if (message.Len() > 0) {
		UUITools::setTextBlockText(this, "Processing_Message", message);
		UUITools::setVisibility(this, "Processing_Message", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "Processing_Message", ESlateVisibility::Collapsed);
	}

	UUITools::setVisibility(this, "ProfileList", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "CreateNew", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "EnterCredentials", ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "Processing", ESlateVisibility::Visible);

	defaultGamePadFocus = nullptr;

	AComplexPlayerController* controller = Cast< AComplexPlayerController >(GetOwningPlayer());
	AComplexHUD* hud = AComplexHUD::get(controller);
	if (hud && controller && controller->usingGamepad) {
		hud->setVirtualFocus(nullptr);
	}
}

void UUserLoginWidget::doRegister() {

	UTextBox* nameInput = Cast< UTextBox >(UUITools::getWidget(this, "CreateNew_Name"));
	if (!nameInput) {
		UDebug::error("Missing CreateNew_Name field.");
		return;
	}
	FString name = nameInput->value;

	UTextBox* passwordInput = Cast< UTextBox >(UUITools::getWidget(this, "CreateNew_Password"));
	if (!passwordInput) {
		UDebug::error("Missing CreateNew_Password field.");
		return;
	}
	FString password = passwordInput->value;

	UBooleanInput* offlineInput = Cast< UBooleanInput >(UUITools::getWidget(this, "CreateNew_Offline"));
	if (!offlineInput) {
		UDebug::error("Missing CreateNew_Offline field.");
		return;
	}
	bool offline = offlineInput->value;

	if (UStringTools::isEmpty(name)) {
		showCreateUser("You must enter a name.");
		return;
	}
	for (UUserProfile* previousProfile : UUserManager::instance->possibleProfiles) {
		if (previousProfile->username.Equals(name)) {
			showCreateUser("A profile with this name already exists.");
			return;
		}
	}

	if (!offline && UStringTools::isEmpty(password)) {
		showCreateUser("You must enter a password.");
		return;
	}

	if (!offline) {
		if (!UUserManager::instance) {
			showCreateUser("User Manager not initialized.");
			return;
		}

		if (UHTTPManager::instance) {

			showProcessing("Registering new user profile...");

			UUserProfile* profile = NewObject< UUserProfile >(UUserManager::instance, UUserManager::instance->getProfileClass());
			profile->username = name;
			profile->password = password;
			profile->local = offline;
			applyCreateProfileOtherInputs(profile);

			UUserLoginTransaction* transaction = Cast< UUserLoginTransaction > ( UHTTPManager::instance->initTransaction(UUserLoginTransaction::StaticClass()) );
			transaction->profile = profile;
			applyCreateProfileTransactionExtraData(transaction, profile);

			TMap<FString, FString > headers;
			TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
			UJsonTools::setString(contents, "game", UGameTools::getGameNameFromWorld(GetWorld()));
			UJsonTools::setString(contents, "username", name);
			UJsonTools::setString(contents, "password", password);
			UJsonTools::setObject(contents, "extraData", transaction->extraData);

			transaction->onSuccess.AddDynamic(this, &UUserLoginWidget::onReceiveRegisterResponse);
			transaction->onFailure.AddDynamic(this, &UUserLoginWidget::onReceiveRegisterResponse);
			UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/register.php", headers, UJsonTools::toString(contents, false) );
		}

		return;
	}

	UUserProfile* profile = NewObject< UUserProfile >(UUserManager::instance, UUserManager::instance->getProfileClass());
	profile->username = name;
	profile->password = password;
	profile->local = offline;
	applyCreateProfileOtherInputs(profile);
	UUserManager::instance->registerNewProfile(GetOwningPlayer(), profile);

	close();
}

void UUserLoginWidget::applyCreateProfileOtherInputs(UUserProfile* profile) {

}

void UUserLoginWidget::applyCreateProfileTransactionExtraData(UUserLoginTransaction* transaction, UUserProfile* profile) {

}

void UUserLoginWidget::doLogin() {

	UTextBox* nameInput = Cast< UTextBox >(UUITools::getWidget(this, "EnterCredentials_Name"));
	if (!nameInput) {
		UDebug::error("Missing EnterCredentials_Name field.");
		return;
	}
	FString name = nameInput->value;

	UTextBox* passwordInput = Cast< UTextBox >(UUITools::getWidget(this, "EnterCredentials_Password"));
	if (!passwordInput) {
		UDebug::error("Missing EnterCredentials_Password field.");
		return;
	}
	FString password = passwordInput->value;

	
	if (UStringTools::isEmpty(name) || name.Len() < 4) {
		showEnterCredentials("You must enter a name, at least 4 characters.");
		return;
	}

	if (UStringTools::isEmpty(password) || name.Len() < 8) {
		showEnterCredentials("You must enter a password, at least 8 characters.");
		return;
	}

	if (!UUserManager::instance) {
		showEnterCredentials("User Manager not initialized.");
		return;
	}

	if (UHTTPManager::instance) {

		showProcessing("Loggin In...");

		UUserProfile* profile = NewObject< UUserProfile >(UUserManager::instance, UUserManager::instance->getProfileClass());
		profile->username = name;
		profile->password = password;

		UUserLoginTransaction* transaction = Cast< UUserLoginTransaction >(UHTTPManager::instance->initTransaction(UUserLoginTransaction::StaticClass()));
		transaction->profile = profile;

		TMap<FString, FString > headers;
		TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
		UJsonTools::setString(contents, "username", name);
		UJsonTools::setString(contents, "password", password);

		transaction->onSuccess.AddDynamic(this, &UUserLoginWidget::onReceiveLoginResponse);
		transaction->onFailure.AddDynamic(this, &UUserLoginWidget::onReceiveLoginResponse);
		UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/login.php", headers, UJsonTools::toString(contents, false));
	}

}


void UUserLoginWidget::selectProfile(UUserWidget* child) {
	UComplexButton* profileButton = Cast<UComplexButton>(child);
	if (profileButton == nullptr) {
		return;
	}

	FString username = profileButton->displayText;
	UUserProfile* profile = nullptr;
	for (UUserProfile* previousProfile : UUserManager::instance->possibleProfiles) {
		if (previousProfile->username.Equals(username)) {
			profile = previousProfile;
			break;
		}
	}

	if (!profile->local) {

		showProcessing("Logging in...");

		UUserLoginTransaction* transaction = Cast< UUserLoginTransaction >(UHTTPManager::instance->initTransaction(UUserLoginTransaction::StaticClass()));
		transaction->profile = profile;

		TMap<FString, FString > headers;
		TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
		UJsonTools::setString(contents, "username", profile->username);
		UJsonTools::setString(contents, "password", profile->password);

		transaction->onSuccess.AddDynamic(this, &UUserLoginWidget::onReceiveLoginResponse);
		transaction->onFailure.AddDynamic(this, &UUserLoginWidget::onReceiveLoginResponse);
		UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/login.php", headers, UJsonTools::toString(contents, false));

		return;
	}

	UUserManager::instance->registerExistingProfile(GetOwningPlayer(), profile);
	close();
}

void UUserLoginWidget::onReceiveRegisterResponse(UHTTPTransaction* transaction, FString response) {

	UUserLoginTransaction* userTransaction = Cast< UUserLoginTransaction >(transaction);
	if (!userTransaction) {
		showCreateUser("Invalid Transaction Type");
		return;
	}


	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(response);
	if (root == nullptr) {
		showCreateUser("Network Error - " + response);
		return;
	}

	FString status = root->GetStringField("status");
	if (status.Equals("INVALID_CREDENTIALS") || status.Equals("INVALID_REQUEST")) {
		showCreateUser("Invalid Credentials");
	}
	else if (status.Equals("ALREADY_EXISTS")) {
		showCreateUser("Account Already Exists");
	}
	else if (status.Equals("SUCCESS")) {

		FString token = root->GetStringField("token");
		userTransaction->profile->token = token;

		UUserManager::instance->registerNewProfile(GetOwningPlayer(), userTransaction->profile);
		close();
	}
	else {
		showCreateUser(status);
	}

}


void UUserLoginWidget::onReceiveLoginResponse(UHTTPTransaction* transaction, FString response) {

	UUserLoginTransaction* userTransaction = Cast< UUserLoginTransaction >(transaction);
	if (!userTransaction) {
		showEnterCredentials("Invalid Transaction Type");
		return;
	}


	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(response);
	if (root == nullptr) {
		showEnterCredentials("Network Error - " + response);
		return;
	}

	FString status = root->GetStringField("status");
	if (status.Equals("INVALID_CREDENTIALS") || status.Equals("INVALID_REQUEST")) {
		showEnterCredentials("Invalid Credentials");
	}
	else if (status.Equals("SUCCESS")) {

		FString extraData = "{" + root->GetStringField("extraData") + "}";
		userTransaction->profile->parseExtraData(extraData);

		FString token = root->GetStringField("token");
		userTransaction->profile->token = token;

		if (UUserManager::instance->profileExistsLocally(userTransaction->profile)) {
			UUserManager::instance->registerExistingProfile(GetOwningPlayer(), userTransaction->profile);
		}
		else {
			UUserManager::instance->registerNewProfile(GetOwningPlayer(), userTransaction->profile);
		}

		close();
	}
	else {
		showEnterCredentials(status);
	}

}