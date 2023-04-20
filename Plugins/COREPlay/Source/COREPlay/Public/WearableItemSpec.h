#pragma once

#include "CoreMinimal.h"
#include "ItemSpec.h"
#include "BodyPart.h"

#include "WearableItemSpec.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UWearableItemSpec : public UItemSpec {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		FString slot;

	UPROPERTY(EditAnywhere)
		UBodyPart* bodyPart = nullptr;

};
