#include "FaceComponent.h"
#include "BodyComponent.h"
#include "CreatureAnimInstance.h"
#include "Creature.h"
#include "BodyPart.h"
#include "ExpressionSet.h"
#include "ResourceCache.h"
#include "SettingsTools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GameFramework/Character.h"

UFaceComponent::UFaceComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UFaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!getMeshComponent()) {
		return;
	}

	active = USettingsTools::getVisualEffectQuality() >= USettingsTools::QUALITY_MEDIUM;
	if (active) {
		active = meshComponent->GetPredictedLODLevel() <= maxMeshLOD;
	}

	if (!active) {
		for (auto& morph : morphs) {
			meshComponent->SetMorphTarget(FName(*morph.Key), 0.0f, true);
		}
		morphs.Empty();
		expressions.Empty();
	}
	else {
		updateExpressions(DeltaTime);

		for (auto& morph : morphs) {
			morphs[morph.Key] = 0.0f;
		}

		TArray< FString > deadExpressions;
		for (auto& expressionEntry : expressions) {
			if (expressionSet == nullptr || !expressionSet->expressions.Contains(expressionEntry.Key)) {
				deadExpressions.Add(expressionEntry.Key);
				continue;
			}
			FExpression& expression = expressionSet->expressions[expressionEntry.Key];

			float durationAmount = 1.0f;
			if (expressionEntry.Value.duration > 0.0f) {
				expressionEntry.Value.time += DeltaTime;
				if (expressionEntry.Value.time >= expressionEntry.Value.duration) {
					deadExpressions.Add(expressionEntry.Key);
					continue;
				}
				if (expressionEntry.Value.blendInEnd > expressionEntry.Value.time) {
					durationAmount = expressionEntry.Value.time / expressionEntry.Value.blendInEnd;
				}
				else if (expressionEntry.Value.blendOutStart < expressionEntry.Value.time) {
					durationAmount = 1.0f - ((expressionEntry.Value.time - expressionEntry.Value.blendOutStart) / expressionEntry.Value.blendOutDuration);
				}
			}

			for (auto& morph : expression.morphs) {
				float newMorphAmount = morph.Value * durationAmount * expressionEntry.Value.amount;
				if (!morphs.Contains(morph.Key)) {
					morphs.Add(morph.Key, newMorphAmount);
				}
				else {
					morphs.Add(morph.Key, FMath::Max(morphs[morph.Key], newMorphAmount));
				}
			}
		}

		TArray< FString > deadMorphs;
		for (auto& morph : morphs) {

			float originalMorphAmount = meshComponent->GetMorphTarget(FName(*morph.Key));
			float finalMorphAmount = FMath::Clamp(UMathTools::lerpFloatByPerc(originalMorphAmount, morph.Value, DeltaTime * 10.0f), 0.0f, 1.0f);

			if (finalMorphAmount <= 0.01f) {
				deadMorphs.Add(morph.Key);
			}
			meshComponent->SetMorphTarget(FName(*morph.Key), finalMorphAmount, true);
		}

		for (FString key : deadMorphs) {
			morphs.Remove(key);
		}
		for (FString key : deadExpressions) {
			expressions.Remove(key);
		}
	}

}


USkeletalMeshComponent* UFaceComponent::getMeshComponent(){

	if (meshComponent == nullptr) {
		UBodyComponent* bodyComponent = Cast< UBodyComponent >(GetOwner()->GetComponentByClass(UBodyComponent::StaticClass()));
		if (bodyComponent == nullptr) {
			return nullptr;
		}
		for (FBodyPartSlot& slot : bodyComponent->slots) {
			UBodyPart* bodyPart = UResourceCache::getBodyPart(slot.bodyPartKey);
			if (bodyPart == nullptr) {
				continue;
			}
			FBodyPartDetail& details = bodyPart->getDetails(slot.key, bodyComponent);
			if (details.tags.Contains("FaceExpressions")) {
				if (bodyComponent->components.Contains(slot.key)) {
					meshComponent = Cast<USkeletalMeshComponent>(bodyComponent->components[slot.key]);
					if (meshComponent) {
						break;
					}
				}
			}
		}
	}
	return meshComponent;
}

UCreatureAnimInstance* UFaceComponent::getAnimInstance() {
	ACreature* creature = Cast< ACreature >(GetOwner());
	if (creature) {
		return creature->getAnimInstance();
	} 
	return nullptr;
}

void UFaceComponent::addExpression(FString key, float amount, float duration, float blendInPerc, float blendOutPerc, bool replace){
	if (!expressionSet || !expressionSet->expressions.Contains(key)) {
		UDebug::error("Could not set expression " + key + " because expressionSet or expression is missing!");
		return;
	}

	if (!replace && expressions.Contains(key) && expressions[key].amount > amount) {
		return;
	}

	FExpressionInstance expression;
	expression.key = key;
	expression.amount = amount;
	expression.duration = duration;
	expression.blendInEnd = duration * blendInPerc;
	expression.blendOutStart = duration * blendOutPerc;
	expression.blendOutDuration = expression.duration - expression.blendOutStart;
	expressions.Add(key, expression);
}

void UFaceComponent::clearExpressions() {
	expressions.Empty();
}

void UFaceComponent::updateExpressions(float deltaTime) {
	if (!active) {
		return;
	}

	if (blinkEnabled) {
		nextBlink -= deltaTime;
		if (nextBlink <= 0.0f) {
			float blinkLength = FMath::FRandRange(0.2f, 0.5f);
			nextBlink = blinkLength + FMath::FRandRange(2.0f, 6.0f);
			addExpression("EyesClosed", 1.0f, blinkLength, 0.5f, 0.5f, true);
		}
	}

	if (microExpressionsEnabled && expressionSet->microExpressions.Num() > 0) {
		nextMicroExpression -= deltaTime;
		if (nextMicroExpression <= 0.0f) {
			nextMicroExpression = FMath::FRandRange(0.1f, 1.0f);
			int microExpressionIndex = FMath::RandRange(0, expressionSet->microExpressions.Num() - 1);
			addExpression(expressionSet->microExpressions[microExpressionIndex], FMath::FRandRange(0.2f, 1.0f) * expressionSet->expressions[expressionSet->microExpressions[microExpressionIndex]].microExpressionAmount, FMath::FRandRange(0.2f, 6.0f), FMath::RandRange(0.05f, 0.25f), FMath::RandRange(0.1f, 0.25f), false);
		}
	}

	if (eyeWander > 0.0f) {
		nextEyeWander -= deltaTime;
		if (nextEyeWander <= 0.0f) {
			nextEyeWander = FMath::FRandRange(2.0f, 4.0f);
			eyeWanderAmounts.X = FMath::FRandRange(-eyeWander, eyeWander);
			eyeWanderAmounts.Y = FMath::FRandRange(-eyeWander, eyeWander);
			if (eyeTarget.isEmpty()) {
				setEyeRotation(eyeWanderAmounts.X, eyeWanderAmounts.Y);
			}
		}
	}

	if (eyeTarget.isPopulated()) {
		FVector2D eyeTargetAmounts = getEyeTargetAmounts();

		if (eyeWander > 0.0f) {
			setEyeRotation(eyeTargetAmounts.X + eyeWanderAmounts.X, eyeTargetAmounts.Y + eyeWanderAmounts.Y);
		}
		else {
			setEyeRotation(eyeTargetAmounts.X, eyeTargetAmounts.Y);
		}
	}
}

void UFaceComponent::setEyeTarget(UObject* object, FVector location) {
	if (eyeTarget.isEmpty()) {
		eyeWanderAmounts = FVector2D::ZeroVector;
	}
	eyeTarget = FTargetDetail(object,location);
}

void UFaceComponent::clearEyeTarget() {
	eyeTarget = FTargetDetail();
	clearEyeRotation();
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		animInstance->clearLookAt(false);
	}
}

FVector2D UFaceComponent::getEyeTargetAmounts() {
	FVector2D result = FVector2D::ZeroVector;

	FTransform headTransform = getHeadTransform();
	FVector headDirection = headTransform.GetRotation().GetUpVector();
	FRotator headRotation = UMathTools::getYawAndPitch(FVector::ZeroVector, headDirection);
	headRotation.Pitch *= -1.0f;

	FRotator targetRotation = UMathTools::getYawAndPitch(headTransform.GetLocation(), eyeTarget.location);
	targetRotation.Pitch *= -1.0f;

	float diffX = UMathTools::getAngleDiff(headRotation.Yaw, targetRotation.Yaw, false) / 90.0f;
	float diffY = UMathTools::getAngleDiff(headRotation.Pitch, targetRotation.Pitch, false) / 90.0f;

	return FVector2D(FMath::Clamp(diffX, -1.0f, 1.0f), FMath::Clamp(diffY, -1.0f, 1.0f));
}

FTransform UFaceComponent::getHeadTransform() {
	ACharacter* character = Cast< ACharacter >(GetOwner());
	if (!character) {
		return FTransform();
	}
	int boneIndex = character->GetMesh()->GetBoneIndex("Head");
	return character->GetMesh()->GetBoneTransform(boneIndex);
}

FVector UFaceComponent::getHeadLocation() {
	return getHeadTransform().GetLocation();
}

void UFaceComponent::setEyeRotation(float x, float y) {
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		animInstance->setEyeTurn(FVector2D(x, y), 1.0f);
	}
}

void UFaceComponent::clearEyeRotation() {
	eyeWanderAmounts = FVector2D::ZeroVector;
	UCreatureAnimInstance* animInstance = getAnimInstance();
	if (animInstance) {
		animInstance->setEyeTurn(FVector2D::ZeroVector, 0.0f);
	}
}

TArray< FString > UFaceComponent::getPossibleExpressionMorphs() {
	TArray< FString > results;
	if (meshComponent) {
		USkeletalMesh* skeletalMesh = Cast< USkeletalMesh >(meshComponent->GetSkinnedAsset());
		if (skeletalMesh) {
			return skeletalMesh->K2_GetAllMorphTargetNames();
		}
	}
	return results;
}

void UFaceComponent::applyPainExpression() {
	clearExpressions();
	nextBlink = 3.0f;
	nextMicroExpression = 5.0f;
	addExpression("Pain", 1.0f, 1.0f, 0.1f, 0.5f, true);
}


void UFaceComponent::applyDeadExpression() {
	clearExpressions();
	stopNaturalExpressions();
	addExpression("Dead", 1.0f, -1.0f, 0.0f, 0.0f, true);
}

void UFaceComponent::stopNaturalExpressions() {
	blinkEnabled = false;
	microExpressionsEnabled = false;
}

void UFaceComponent::applyNaturalExpressions() {
	blinkEnabled = true;
	microExpressionsEnabled = true;
}

UFaceComponent* UFaceComponent::get(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	return Cast< UFaceComponent >(actor->GetComponentByClass(UFaceComponent::StaticClass()));
}
