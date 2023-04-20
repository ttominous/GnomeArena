#pragma once

#include "CoreMinimal.h"
#include "UserLoginWidget.h"

#include "GnomeUserLoginWidget.generated.h"


UCLASS()
class GNOMEARENA_API UGnomeUserLoginWidget : public UUserLoginWidget {

	GENERATED_BODY()

public:

	virtual void applyCreateProfileOtherInputs(UUserProfile* profile) override;
	virtual void applyCreateProfileTransactionExtraData(UUserLoginTransaction* transaction, UUserProfile* profile) override;

};