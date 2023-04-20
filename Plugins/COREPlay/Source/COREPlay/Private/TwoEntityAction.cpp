#include "TwoEntityAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "CollisionTools.h"
#include "MathTools.h"
#include "Debug.h"



bool UTwoEntityAction::processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) {
	FString type = payload.getString("type");
	if (type.Equals(FPayload::TYPE_ANIM_END)) {
		FString animSequence = payload.getString("animSequence");

		UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
		if (animSequence.Equals(twoEntityActionInstance->leaderAnimSequence)) {
			processLeaderAnimationFinished(instance, component);
		}
		else if (animSequence.Equals(twoEntityActionInstance->followerAnimSequence)) {
			processFollowerAnimationFinished(instance, component);
		}
	}
	else if (type.Equals(FPayload::TYPE_ANIM_PING)) {
		FString animSequence = payload.getString("animSequence");

		UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
		if (animSequence.Equals(twoEntityActionInstance->leaderAnimSequence)) {
			processLeaderAnimationPing(instance, component, payload);
		}
		else if (animSequence.Equals(twoEntityActionInstance->followerAnimSequence)) {
			processFollowerAnimationPing(instance, component, payload);
		}
	}
	return true;
}


void UTwoEntityAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {
	Super::processStart(instance, component, target);

	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
	if (twoEntityActionInstance->started) {
		return;
	}

	ACreature* leaderCreature = Cast< ACreature >(component->GetOwner());
	if (!leaderCreature) {
		component->stopAction(key, instance);
		return;
	}

	ACreature* followerCreature = Cast< ACreature >(target.getCreature(component->GetWorld()));
	if (!followerCreature) {
		component->stopAction(key, instance);
		return;
	}

	twoEntityActionInstance->leader = leaderCreature;
	twoEntityActionInstance->leaderActionComponent = leaderCreature->getActionComponent();

	twoEntityActionInstance->follower = followerCreature;
	twoEntityActionInstance->followerActionComponent = followerCreature->getActionComponent();
	twoEntityActionInstance->followerActionComponent->bindSharedAction(this, instance);

	twoEntityActionInstance->started = true;
}

void UTwoEntityAction::processEnd(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
	if (twoEntityActionInstance->ended) {
		return;
	}
	twoEntityActionInstance->ended = true;
	processActualEnd(instance, component);
}

void UTwoEntityAction::processActualEnd(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
	if (twoEntityActionInstance->leaderActionComponent) {
		twoEntityActionInstance->leaderActionComponent->stopAction(key, instance);
	}
	if (twoEntityActionInstance->followerActionComponent) {
		twoEntityActionInstance->followerActionComponent->stopAction(key, instance);
	}
}


void UTwoEntityAction::processLeaderAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

}

void UTwoEntityAction::processFollowerAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

}

void UTwoEntityAction::processLeaderAnimationFinished(UActionInstance* instance, UActionComponent* component) {

	/*UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);

	if (component == twoEntityActionInstance->leaderActionComponent) {
		ACreature* creature = Cast< ACreature >(twoEntityActionInstance->leader);
		if (creature) {
			UCreatureAnimInstance* animInstance = creature->getAnimInstance();
			if (animInstance) {
				animInstance->processPlayAnimEnd(animationData);
			}
		}
	}
	else if (component == twoEntityActionInstance->followerActionComponent) {
		ACreature* creature = Cast< ACreature >(twoEntityActionInstance->follower);
		if (creature) {
			UCreatureAnimInstance* animInstance = creature->getAnimInstance();
			if (animInstance) {
				animInstance->processPlayAnimEnd(animationData);
			}
		}
	}*/
}

void UTwoEntityAction::processFollowerAnimationFinished(UActionInstance* instance, UActionComponent* component) {

}

void UTwoEntityAction::processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) {
	Super::processTick(deltaTime, instance, component);

	if (!isLeader(instance, component)) {
		return;
	}

	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
	if (!twoEntityActionInstance->initialPositioned) {
		bool positionedLeader = !faceLeader || positionLeader(instance,component, deltaTime);
		bool positionedFollower = !faceFollower || positionFollower(instance, component, deltaTime);
		twoEntityActionInstance->initialPositioned = positionedLeader && positionedFollower;
		return;
	}
	processUpdate(deltaTime, instance, component);
}

void UTwoEntityAction::processUpdate(float deltaTime, UActionInstance* instance, UActionComponent* component) {

}


bool UTwoEntityAction::positionLeader(UActionInstance* instance, UActionComponent* component, float deltaTime) {
	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);

	ACreature* leader = Cast< ACreature >(twoEntityActionInstance->leader);
	ACreature* follower = Cast< ACreature >(twoEntityActionInstance->follower);

	float targetYaw = UMathTools::getYaw(leader->GetActorLocation(), follower->GetActorLocation()) - 90.0f;
	leader->bodyYaw = UMathTools::lerpAngleByDegrees(leader->bodyYaw, targetYaw, deltaTime * 300.0f);

	return UMathTools::getAngleDiff(leader->bodyYaw, targetYaw, true) <= 2.0f;
}

bool UTwoEntityAction::positionFollower(UActionInstance* instance, UActionComponent* component, float deltaTime) {
	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);

	ACreature* leader = Cast< ACreature >(twoEntityActionInstance->leader);
	ACreature* follower = Cast< ACreature >(twoEntityActionInstance->follower);

	float targetYaw = UMathTools::getYaw(follower->GetActorLocation(), leader->GetActorLocation()) - 90.0f;
	follower->bodyYaw = UMathTools::lerpAngleByDegrees(follower->bodyYaw, targetYaw, deltaTime * 300.0f);

	return UMathTools::getAngleDiff(follower->bodyYaw, targetYaw, true) <= 2.0f;
}

bool UTwoEntityAction::isLeader(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityActionInstance* twoEntityActionInstance = Cast< UTwoEntityActionInstance >(instance);
	return twoEntityActionInstance->leaderActionComponent == component;
}



UClass* UTwoEntityAction::getInstanceClass(UActionComponent* component) {
	return UTwoEntityActionInstance::StaticClass();
}