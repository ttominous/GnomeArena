#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "HTTPManager.generated.h"


UCLASS()
class COREPLAY_API UHTTPTransaction : public UObject {

	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResponseReceived, class UHTTPTransaction*, transaction, FString, response);

	UPROPERTY()
		FOnResponseReceived onSuccess;

	UPROPERTY()
		FOnResponseReceived onFailure;

	void handleResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool success);
};




UCLASS()
class COREPLAY_API UHTTPManager : public UObject {

	GENERATED_BODY()

public:

	static UHTTPManager* instance;

	UPROPERTY()
		FString externalIP = "";

	UPROPERTY()
		TArray< UHTTPTransaction* > transactions;

	UPROPERTY()
		TMap< FString, float > waitTimers;

	UPROPERTY()
		float waitAll = 1.0f;

	virtual void update(float deltaTime);
	virtual bool ready(FString timerKey);
	virtual void wait(FString timerKey, float time = 10.0f);

	virtual UHTTPTransaction* initTransaction();
	virtual UHTTPTransaction* initTransaction(UClass* transactionClass);
	virtual void doSimpleURLCall(UHTTPTransaction* transaction, FString url);
	virtual void doJsonPOST(UHTTPTransaction* transaction, FString url, TMap<FString, FString > headers, TMap<FString, FString > contents);
	virtual void doJsonPOST(UHTTPTransaction* transaction, FString url, TMap<FString, FString > headers, FString contents);

	virtual FString getExternalIP();

	UFUNCTION()
		virtual void parseExternalIP(UHTTPTransaction* transaction, FString response);


};