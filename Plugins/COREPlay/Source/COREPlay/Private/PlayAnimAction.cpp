#include "PlayAnimAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "DualAxisAnimInstance.h"
#include "Debug.h"



bool UPlayAnimAction::processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) {
	FString type = payload.getString("type");
	if (type.Equals(FPayload::TYPE_ANIM_END)) {
		FString animSequence = payload.getString("animSequence");

		UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
		if (animSequence.Equals(playAnimInstance->animSequence)) {
			processAnimationFinished(instance, component);
		}
	}
	else if (type.Equals(FPayload::TYPE_ANIM_PING)) {
		FString animSequence = payload.getString("animSequence");

		UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
		if (animSequence.Equals(playAnimInstance->animSequence)) {
			processAnimationPing(instance, component, payload);
		}
	}
	return true;
}

void UPlayAnimAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {
	Super::processStart(instance, component, target);

	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	playAnimInstance->animSequence = playAnimation(playedAnimation, creature);

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (animInstance) {
		animInstance->processPlayAnimStart(playedAnimation);
	}
	
	//UDebug::print("Started animation: " + playAnimInstance->actionSequence);
}

void UPlayAnimAction::processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) {
	Super::processTick(deltaTime, instance, component);

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (creature) {
		UCreatureAnimInstance* animInstance = creature->getAnimInstance();
		if (animInstance && !animInstance->isPlayingAnimation(playedAnimation.slot)) {
			processAnimationFinished(instance, component);
			return;
		}
	}
}


void UPlayAnimAction::processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

}

void UPlayAnimAction::processAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (creature) {
		UCreatureAnimInstance* animInstance = creature->getAnimInstance();
		if (animInstance) {
			animInstance->processPlayAnimEnd(playedAnimation);
			playAnimInstance->animSequence = "";
		}
	}

	component->stopAction(key, instance);
}

void UPlayAnimAction::processEnd(UActionInstance* instance, UActionComponent* component) {

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	if (playAnimInstance && playAnimInstance->animSequence.Len() > 0) {
		ACreature* creature = Cast< ACreature >(component->GetOwner());
		if (creature) {
			UCreatureAnimInstance* animInstance = creature->getAnimInstance();
			if (animInstance) {
				animInstance->processPlayAnimEnd(playedAnimation);
			}
		}
	}
	Super::processEnd(instance, component);
}

UClass* UPlayAnimAction::getInstanceClass(UActionComponent* component) {
	return UPlayAnimActionInstance::StaticClass();
}

