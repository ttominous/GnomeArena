#include "AnimStateAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "Debug.h"


void UAnimStateAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {
	Super::processStart(instance, component, target);

	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	if (animationData.animSequenceKey.Len() > 0) {
		playAnimation(animationData, creature);
	}

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (animInstance) {
		animInstance->processPlayAnimStart(animationData);
	}
}

void UAnimStateAction::processEnd(UActionInstance* instance, UActionComponent* component) {

	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (animInstance) {
		animInstance->processPlayAnimEnd(animationData);
	}
}
