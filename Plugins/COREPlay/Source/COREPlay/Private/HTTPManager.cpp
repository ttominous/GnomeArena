#include "HTTPManager.h"
#include "FileTools.h"
#include "Debug.h"

UHTTPManager* UHTTPManager::instance = nullptr;

void UHTTPTransaction::handleResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool success) {
	FString responseString = response->GetContentAsString();
	
	if (responseString.Len() == 0) {
		responseString = FString::FromInt( response->GetResponseCode() );
	}

	if (success && onSuccess.IsBound()) {
		onSuccess.Broadcast(this, responseString);
	}
	if (!success && onFailure.IsBound()) {
		onFailure.Broadcast(this, responseString);
	}
	if (UHTTPManager::instance) {
		UHTTPManager::instance->transactions.Remove(this);
	}
}




void UHTTPManager::update(float deltaTime) {

	if (waitAll > 0.0f) {
		waitAll -= deltaTime;
	}

	TArray< FString > deadTimers;
	for (auto& timer : waitTimers) {
		waitTimers[timer.Key] -= deltaTime;
		if (waitTimers[timer.Key] <= 0.0f) {
			deadTimers.Add(timer.Key);
		}
	}
	for (FString deadTimer : deadTimers) {
		waitTimers.Remove(deadTimer);
	}
}

bool UHTTPManager::ready(FString timerKey) {
	return waitAll <= 0.0f && !waitTimers.Contains(timerKey);
}

void UHTTPManager::wait(FString timerKey, float time) {
	waitTimers.Add(timerKey, time);
}

UHTTPTransaction* UHTTPManager::initTransaction() {
	return initTransaction(UHTTPTransaction::StaticClass());
}

UHTTPTransaction* UHTTPManager::initTransaction(UClass* transactionClass) {
	UHTTPTransaction* transaction = NewObject< UHTTPTransaction >(this, transactionClass);
	transactions.Add(transaction);
	return transaction;
}

void UHTTPManager::doSimpleURLCall(UHTTPTransaction* transaction, FString url) {
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> request = httpModule.CreateRequest();
	request->SetURL(url);
	request->OnProcessRequestComplete().BindUObject(transaction, &UHTTPTransaction::handleResponse);
	request->ProcessRequest();
}

void UHTTPManager::doJsonPOST(UHTTPTransaction* transaction, FString url, TMap<FString, FString > headers, TMap<FString, FString > contents) {

	FString content = "{";
	bool firstValue = true;
	for (auto& variable : contents) {
		if (firstValue) {
			firstValue = false;
		}
		else {
			content += ",";
		}
		content += "\"" + variable.Key + "\":\"" + variable.Value + "\"";
	}
	content += "}";

	doJsonPOST(transaction, url, headers, content);
}

void UHTTPManager::doJsonPOST(UHTTPTransaction* transaction, FString url, TMap<FString, FString > headers, FString content) {

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> request = httpModule.CreateRequest();
	request->SetURL(url);

	request->SetVerb("POST");
	request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	for (auto& header : headers) {
		request->SetHeader(header.Key, header.Value);
	}

	request->SetContentAsString(content);

	request->OnProcessRequestComplete().BindUObject(transaction, &UHTTPTransaction::handleResponse);
	request->ProcessRequest();
}

FString UHTTPManager::getExternalIP() {
	if (externalIP.Len() == 0) {
		if (ready("ExternalIP")) {
			UHTTPTransaction* transaction = initTransaction();
			transaction->onSuccess.AddDynamic(this, &UHTTPManager::parseExternalIP);
			doSimpleURLCall(transaction, "https://api.ipify.org");
			wait("ExternalIP");
		}
	}
	return externalIP;
}

void UHTTPManager::parseExternalIP(UHTTPTransaction* transaction, FString response) {
	externalIP = response;
}