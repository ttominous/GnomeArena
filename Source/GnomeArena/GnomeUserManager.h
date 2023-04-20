#pragma once

#include "CoreMinimal.h"
#include "UserManager.h"

#include "GnomeUserManager.generated.h"


UCLASS()
class GNOMEARENA_API UGnomeUserManager : public UUserManager {

	GENERATED_BODY()

public:

	virtual UClass* getProfileClass();
};