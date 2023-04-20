#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HTTPManager.h"

#include "HTTPTools.generated.h"

UCLASS()
class COREPLAY_API UHTTPTools : public UObject {

	GENERATED_BODY()

public:

	static FString getExternalIP();

};