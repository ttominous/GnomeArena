#include "AnimationSet.h"
#include "Debug.h"

const FString UAnimationSet::ANIM_SLOT_FULL_BODY = "FullBody";
const FString UAnimationSet::ANIM_SLOT_UPPER_BODY = "UpperBody";
const FString UAnimationSet::ANIM_SLOT_HIT = "Hit";
const FString UAnimationSet::ANIM_SLOT_FACE = "Face";

UAnimSequence* UAnimationSet::getSequence(FString animKey) {
	if (sequences.Contains(animKey)) {
		return sequences[animKey];
	}
	return nullptr;
}

UBlendSpace* UAnimationSet::getBlendSpace(FString animKey) {
	if (blendSpaces.Contains(animKey)) {
		return blendSpaces[animKey];
	}
	return nullptr;
}


UBlendSpace1D* UAnimationSet::getBlendSpace1D(FString animKey) {
	if (blendSpaces1D.Contains(animKey)) {
		return blendSpaces1D[animKey];
	}
	return nullptr;
}

UAnimMontage* UAnimationSet::getMontage(FString animKey) {
	if (montages.Contains(animKey)) {
		return montages[animKey];
	}
	return nullptr;
}


UAimOffsetBlendSpace* UAnimationSet::getAimOffset(FString animKey) {
	if (aimOffsets.Contains(animKey)) {
		return aimOffsets[animKey];
	}
	return nullptr;
}


UAimOffsetBlendSpace1D* UAnimationSet::getAimOffset1D(FString animKey) {
	if (aimOffsets1D.Contains(animKey)) {
		return aimOffsets1D[animKey];
	}
	return nullptr;
}




float UAnimationSet::getSequenceLength(FString animKey) {
	UAnimSequence* sequence = getSequence(animKey);
	if (!sequence) {
		return 0.0f;
	}
	return sequence->GetPlayLength();
}

float UAnimationSet::getSequenceLength(FString animKey, float atRate) {
	UAnimSequence* sequence = getSequence(animKey);
	if (!sequence) {
		return 0.0f;
	}
	return sequence->GetPlayLength() / atRate;
}

float UAnimationSet::getBlendSpace1DLength(FString animKey) {
	UBlendSpace1D* blendSpace = getBlendSpace1D(animKey);
	if (!blendSpace) {
		return 0.0f;
	}
	return blendSpace->AnimLength;
}

float UAnimationSet::getBlendSpace1DLength(FString animKey, float atRate) {
	UBlendSpace1D* blendSpace = getBlendSpace1D(animKey);
	if (!blendSpace) {
		return 0.0f;
	}
	return blendSpace->AnimLength / atRate;
}

float UAnimationSet::getBlendSpaceLength(FString animKey) {
	UBlendSpace* blendSpace = getBlendSpace(animKey);
	if (!blendSpace) {
		return 0.0f;
	}
	return blendSpace->AnimLength;
}

float UAnimationSet::getBlendSpaceLength(FString animKey, float atRate) {
	UBlendSpace* blendSpace = getBlendSpace(animKey);
	if (!blendSpace) {
		return 0.0f;
	}
	return blendSpace->AnimLength / atRate;
}


float UAnimationSet::getAimOffset1DLength(FString animKey) {
	UAimOffsetBlendSpace1D* aimOffset = getAimOffset1D(animKey);
	if (!aimOffset) {
		return 0.0f;
	}
	return aimOffset->AnimLength;
}

float UAnimationSet::getAimOffset1DLength(FString animKey, float atRate) {
	UAimOffsetBlendSpace1D* aimOffset = getAimOffset1D(animKey);
	if (!aimOffset) {
		return 0.0f;
	}
	return aimOffset->AnimLength / atRate;
}

float UAnimationSet::getAimOffsetLength(FString animKey) {
	UAimOffsetBlendSpace* aimOffset = getAimOffset(animKey);
	if (!aimOffset) {
		return 0.0f;
	}
	return aimOffset->AnimLength;
}

float UAnimationSet::getAimOffsetLength(FString animKey, float atRate) {
	UAimOffsetBlendSpace* aimOffset = getAimOffset(animKey);
	if (!aimOffset) {
		return 0.0f;
	}
	return aimOffset->AnimLength / atRate;
}

int UAnimationSet::getAnimCount(FString key) {
	if (animCounts.Contains(key)) {
		return animCounts[key];
	}
	return 0;
}

int UAnimationSet::getRandomInt(FString key) {
	if (animCounts.Contains(key)) {
		return FMath::RandRange(1, animCounts[key]);
	}
	return 1;
}



UAnimationSet* UAnimationSet::parseXML(FXmlNode* root) {
	UAnimationSet* animationSet = NewObject< UAnimationSet >();

	TArray<FXmlNode*> nodes = root->GetChildrenNodes();
	for (FXmlNode* node : nodes) {
		FString tag = node->GetTag().ToUpper();

		FString key = node->GetAttribute("key");
		FString path = node->GetAttribute("path");

		if (tag.Equals("SEQUENCE")) {
			UAnimSequence* sequence = LoadObject< UAnimSequence >(animationSet, *path, NULL, LOAD_None, NULL);
			animationSet->sequences.Add(key, sequence);
		}
		else if (tag.Equals("BLENDSPACE1D")) {
			UBlendSpace1D* blendspace = LoadObject< UBlendSpace1D >(animationSet, *path, NULL, LOAD_None, NULL);
			animationSet->blendSpaces1D.Add(key, blendspace);
		}
		else if (tag.Equals("BLENDSPACE")) {
			UBlendSpace* blendspace = LoadObject< UBlendSpace >(animationSet, *path, NULL, LOAD_None, NULL);
			animationSet->blendSpaces.Add(key, blendspace);
		}
	}

	return animationSet;
}

void UAnimationSet::copyTo(UAnimationSet* target) {
	for (auto& asset : sequences) {
		target->sequences.Add(asset.Key, asset.Value);
	}
	for (auto& asset : blendSpaces1D) {
		target->blendSpaces1D.Add(asset.Key, asset.Value);
	}
	for (auto& asset : blendSpaces) {
		target->blendSpaces.Add(asset.Key, asset.Value);
	}
}