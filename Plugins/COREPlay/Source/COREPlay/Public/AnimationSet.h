#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpace.h"
#include "Animation/BlendSpace1D.h"
#include "Animation/AnimMontage.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AimOffsetBlendSpace1D.h"

#include "XMLTools.h"

#include "AnimationSet.generated.h"


UCLASS(Blueprintable)
class COREPLAY_API UAnimationSet : public UDataAsset {

	GENERATED_BODY()

public:

	static const FString ANIM_SLOT_FULL_BODY;
	static const FString ANIM_SLOT_UPPER_BODY;
	static const FString ANIM_SLOT_HIT;
	static const FString ANIM_SLOT_FACE;

	UAnimationSet() {
	}

	UPROPERTY(EditAnywhere)
		TMap< FString, UAnimSequence* > sequences;

	UPROPERTY(EditAnywhere)
		TMap< FString, UBlendSpace* > blendSpaces;

	UPROPERTY(EditAnywhere)
		TMap< FString, UBlendSpace1D* > blendSpaces1D;

	UPROPERTY(EditAnywhere)
		TMap< FString, UAnimMontage* > montages;

	UPROPERTY(EditAnywhere)
		TMap< FString, UAimOffsetBlendSpace* > aimOffsets;

	UPROPERTY(EditAnywhere)
		TMap< FString, UAimOffsetBlendSpace1D* > aimOffsets1D;

	UPROPERTY(EditAnywhere)
		TMap< FString, int > animCounts;

	UAnimSequence* getSequence(FString animKey);
	UBlendSpace* getBlendSpace(FString animKey);
	UBlendSpace1D* getBlendSpace1D(FString animKey);
	UAnimMontage* getMontage(FString animKey);
	UAimOffsetBlendSpace* getAimOffset(FString animKey);
	UAimOffsetBlendSpace1D* getAimOffset1D(FString animKey);
	int getAnimCount(FString key);
	int getRandomInt(FString key);

	float getSequenceLength(FString animKey);
	float getSequenceLength(FString animKey, float atRate);
	float getBlendSpace1DLength(FString animKey);
	float getBlendSpace1DLength(FString animKey, float atRate);
	float getBlendSpaceLength(FString animKey);
	float getBlendSpaceLength(FString animKey, float atRate);
	float getAimOffset1DLength(FString animKey);
	float getAimOffset1DLength(FString animKey, float atRate);
	float getAimOffsetLength(FString animKey);
	float getAimOffsetLength(FString animKey, float atRate);

	static UAnimationSet* parseXML(FXmlNode* root);
	void copyTo(UAnimationSet* target);

};
