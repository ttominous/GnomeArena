#include "Action.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "DualAxisAnimInstance.h"
#include "Creature.h"
#include "Debug.h"


bool UAction::allowMove(UActionInstance* instance) {
	return allowMovement;
}

bool UAction::allowJump(UActionInstance* instance) {
	return allowJumping;
}

bool UAction::allowSprint(UActionInstance* instance) {
	return allowSprinting;
}

bool UAction::allowAim(UActionInstance* instance) {
	return allowAiming;
}

bool UAction::allowSwitchHands(UActionInstance* instance) {
	return allowHandSwitching;
}

bool UAction::allowTurn(UActionInstance* instance) {
	return allowTurning;
}

bool UAction::allowWeaponHandSnap(UActionInstance* instance) {
	return allowWeaponHandSnapping;
}

bool UAction::allowCrouch(UActionInstance* instance) {
	return allowCrouching;
}

float UAction::getAimAccuracy(UActionInstance* instance) {
	return aimAccuracyMultiplier;
}

bool UAction::canDoAction(AActor* actor) {
	return true;
}

float UAction::getCompletionPerc(UActionInstance* instance, UActionComponent* component) {
	return 0.0f;
}

bool UAction::processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) {
	return true;
}

void UAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {
	if (!allowCrouch(instance)) {
		ADualAxisCreature* creature = Cast<ADualAxisCreature>(component->GetOwner());
		if (creature) {
			creature->crouched = false;
		}
	}
}

void UAction::processDeferredStart(UActionInstance* instance, UActionComponent* component) {

}

void UAction::processExecute(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {

}

void UAction::processEnd(UActionInstance* instance, UActionComponent* component) {

}

void UAction::processInterupt(UActionInstance* instance, UActionComponent* component, FVector targetPosition) {
	component->stopAction(key, instance);
}

void UAction::processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) {

}

void UAction::processInputPress(FString inputKey, UActionInstance* instance, UActionComponent* component) {

}

void UAction::processInputRelease(FString inputKey, UActionInstance* instance, UActionComponent* component) {

}

bool UAction::isRelevantInput(FString inputKey) {
	return false;
}

FString UAction::playAnimation(FPlayAnimationData& animationData, ACreature* creature) {
	if (creature) {

		UAnimSequence* sequence = animationData.animSequence;
		if (sequence == nullptr) {

			FString animKey = animationData.animSequenceKey;
			UDualAxisAnimInstance* animInstance = Cast< UDualAxisAnimInstance >(creature->getAnimInstance());
			if (animInstance) {
				if (animationData.addLocomotionStateIndicator) {
					animKey = animInstance->addLocomotionState(animKey);
				}
				if (animationData.addHandIndicator) {
					animKey = animInstance->addHand(animKey);
				}
			}

			sequence = creature->getAnimSequence(animKey);
			if (sequence == nullptr) {
				return "";
			}
		}

		creature->playAnimSequence(sequence, animationData.playRate, animationData.startTime, animationData.blendIn, animationData.blendOut, animationData.loops, animationData.slot);
		return sequence->GetName();
	}
	return "";
}

UClass* UAction::getInstanceClass(UActionComponent* component) {
	return UActionInstance::StaticClass();
}

UActionInstance* UAction::spawnInstance(UActionComponent* component) {
	UActionInstance* instance = NewObject< UActionInstance >(component, getInstanceClass(component));
	instance->action = this;
	return instance;
}

void UAction::writeXML(FXMLBuilderNode& rootNode, UActionInstance* instance, UActionComponent* component) {

}

void UAction::readXML(FXmlNode* rootNode, UActionInstance* instance, UActionComponent* component) {

}