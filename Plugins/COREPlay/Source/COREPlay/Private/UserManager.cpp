#include "UserManager.h"
#include "PlayerRecord.h"
#include "FileTools.h"
#include "ControllerTools.h"
#include "GameTools.h"
#include "Debug.h"

UUserManager* UUserManager::instance = nullptr;


UUserLoginTransaction::UUserLoginTransaction() {
	extraData = UJsonTools::makeObject();
}






void UUserManager::update(float deltaTime) {

}

void UUserManager::loadProfiles() {

	FString savePath = UFileTools::getRootSavePath() + "/UserProfiles.txt";
	if (!UFileTools::fileExist(savePath)) {
		loaded = true;
		return;
	}

	FXmlNode* rootNode = UXMLTools::getXMLRoot(savePath, false);
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();

	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("USER")) {
			UUserProfile* profile = NewObject< UUserProfile >(this, getProfileClass());
			profile->readXML(childNode);
			possibleProfiles.Add(profile);
		}
	}

	lastProfileKey = UXMLTools::getAttributeString(rootNode, "LastProfile");
	lastTestProfileKey = UXMLTools::getAttributeString(rootNode, "LastTestProfile");

	UUserProfile* autoLoadProfile = nullptr;

	if (GetWorld()->WorldType == EWorldType::PIE) {
		for (UUserProfile* profile : possibleProfiles) {
			if (profile->username.Equals(lastTestProfileKey)) {
				autoLoadProfile = profile;
			}
		}
	}
	else {
		for (UUserProfile* profile : possibleProfiles) {
			if (profile->username.Equals(lastProfileKey)) {
				autoLoadProfile = profile;
			}
		}
	}

	
	if (autoLoadProfile) {

		if (autoLoadProfile->local) {
			loadedProfiles.Add(0, autoLoadProfile);
		}
		else {
			autoLoggingIn = true;

			UUserLoginTransaction* transaction = Cast< UUserLoginTransaction >(UHTTPManager::instance->initTransaction(UUserLoginTransaction::StaticClass()));
			transaction->profile = autoLoadProfile;

			TMap<FString, FString > headers;
			TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
			UJsonTools::setString(contents, "username", autoLoadProfile->username);
			UJsonTools::setString(contents, "password", autoLoadProfile->password);

			transaction->onSuccess.AddDynamic(this, &UUserManager::onReceiveLoginSuccess);
			transaction->onFailure.AddDynamic(this, &UUserManager::onReceiveLoginFailure);
			UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/login.php", headers, UJsonTools::toString(contents, false));
		}
	}

	loaded = true;
}

void UUserManager::saveProfiles() {
	FString savePath = UFileTools::getRootSavePath() + "/UserProfiles.txt";

	FXMLBuilder fileBuilder;

	for (UUserProfile* profile : possibleProfiles) {
		FXMLBuilderNode& profileNode = fileBuilder.addNode("User");
		profile->writeXML(profileNode);
	}

	fileBuilder.root.setAttribute("LastProfile", lastProfileKey);
	fileBuilder.root.setAttribute("LastTestProfile", lastTestProfileKey);

	if (GetWorld()->WorldType == EWorldType::PIE && GPlayInEditorID > 0) {
		FString altProfileKey = "LastProfile" + FString::FromInt(GPlayInEditorID + 1);
		FString altProfileValue = "";
		if (loadedProfiles.Num() > 0) {
			altProfileValue = loadedProfiles[0]->username;
		}
		fileBuilder.root.setAttribute(altProfileKey, altProfileValue);
	}

	UFileTools::saveFile(savePath, fileBuilder.toString());
}

UClass* UUserManager::getProfileClass() {
	return UUserProfile::StaticClass();
}

UUserProfile* UUserManager::getProfile(APlayerController* controller) {
	if (controller == nullptr) {
		return nullptr;
	}
	int localPlayerIndex = UControllerTools::getLocalPlayerIndex(controller);
	return loadedProfiles.Contains(localPlayerIndex) ? loadedProfiles[localPlayerIndex] : nullptr;
}

UUserProfile* UUserManager::getProfileStatic(APlayerController* controller){
	if (instance) {
		return instance->getProfile(controller);
	}
	return nullptr;
}

bool UUserManager::profileExistsLocally(UUserProfile* profile){
	for (UUserProfile* otherProfile : possibleProfiles) {
		if (otherProfile->username.Equals(profile->username)) {
			return true;
		}
	}
	return false;
}

void UUserManager::registerNewProfile(APlayerController* controller, UUserProfile* profile) {
	if (controller == nullptr || profile == nullptr) {
		return;
	}
	int localPlayerIndex = UControllerTools::getLocalPlayerIndex(controller);
	possibleProfiles.Add(profile);
	loadedProfiles.Add(localPlayerIndex, profile);

	UPlayerRecord* playerRecord = UPlayerRecord::get(controller);
	if (playerRecord) {
		playerRecord->userProfile = profile;
	}
	saveProfiles();
}

void UUserManager::registerExistingProfile(APlayerController* controller, UUserProfile* profile) {
	if (controller == nullptr || profile == nullptr) {
		return;
	}

	int localPlayerIndex = UControllerTools::getLocalPlayerIndex(controller);
	loadedProfiles.Add(localPlayerIndex, profile);

	if (localPlayerIndex == 0) {
		if (GetWorld()->WorldType == EWorldType::PIE) {
			lastTestProfileKey = profile->username;
		}
		else {
			lastProfileKey = profile->username;
		}
	}

	UPlayerRecord* playerRecord = UPlayerRecord::get(controller);
	if (playerRecord) {
		playerRecord->userProfile = profile;
	}

	saveProfiles();
}


void UUserManager::onReceiveLoginSuccess(UHTTPTransaction* transaction, FString response) {
	autoLoggingIn = false;

	UUserLoginTransaction* userTransaction = Cast< UUserLoginTransaction >(transaction);
	if (!userTransaction) {
		return;
	}

	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(response);
	if (root == nullptr) {
		return;
	}

	FString status = root->GetStringField("status");
	if (status.Equals("SUCCESS")) {
		FString extraData = "{" + root->GetStringField("extraData") + "}";
		userTransaction->profile->parseExtraData(extraData);

		FString token = root->GetStringField("token");
		userTransaction->profile->token = token;

		loadedProfiles.Add(0, userTransaction->profile);
	}

}

void UUserManager::onReceiveLoginFailure(UHTTPTransaction* transaction, FString response) {
	autoLoggingIn = false;
}

void UUserManager::ping(float deltaTime, FString activityString, FString matchName, FString levelName, FString matchStatus, int matchPlayerCount, int matchPlayerMax) {
	pingTime -= deltaTime;
	if (pingTime <= 0.0f) {

		for (auto profile : loadedProfiles) {
			if (!profile.Value->local) {

				UHTTPTransaction* transaction = UHTTPManager::instance->initTransaction(UUserLoginTransaction::StaticClass());
				transaction->onSuccess.AddDynamic(this, &UUserManager::onPingResponse);
				transaction->onFailure.AddDynamic(this, &UUserManager::onPingResponse);

				TMap<FString, FString > headers;
				TSharedPtr< FJsonObject > contents = UJsonTools::makeObject();
				UJsonTools::setString(contents, "username", profile.Value->username);
				UJsonTools::setString(contents, "token", profile.Value->token);
				UJsonTools::setString(contents, "activity", activityString);
				UJsonTools::setString(contents, "matchName", matchName);
				UJsonTools::setString(contents, "levelName", levelName);
				UJsonTools::setString(contents, "matchStatus", matchStatus);
				UJsonTools::setNumeric(contents, "matchPlayerCount", matchPlayerCount);
				UJsonTools::setNumeric(contents, "matchPlayerMax", matchPlayerMax);
				UJsonTools::setString(contents, "gameName", UGameTools::getGameNameFromWorld(GetWorld()));

				UHTTPManager::instance->doJsonPOST(transaction, "http://socgames.website/ping.php", headers, UJsonTools::toString(contents, false));
			}
		}

		pingTime = pingFrequency;
	}
}


void UUserManager::onPingResponse(UHTTPTransaction* transaction, FString response) {
	TSharedPtr<FJsonObject> root = UJsonTools::parseJSON(response);
	if (root == nullptr) {
		UDebug::error("Network Error - " + response);
		return;
	}
	FString status = root->GetStringField("status");
	if (!status.Equals("SUCCESS")) {
		UDebug::error("Error: " + status);
	}
}