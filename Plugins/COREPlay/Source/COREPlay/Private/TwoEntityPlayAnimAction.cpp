#include "TwoEntityPlayAnimAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "CollisionTools.h"
#include "Debug.h"


void UTwoEntityPlayAnimAction::processUpdate(float deltaTime, UActionInstance* instance, UActionComponent* component) {
	UTwoEntityPlayAnimActionInstance* playAnimInstance = Cast< UTwoEntityPlayAnimActionInstance >(instance);

	if (!playAnimInstance->animationsStarted) {

		assignAnimKeys(instance, component);

		if (!leaderAnimations.Contains(playAnimInstance->leaderAnimKey)) {
			UDebug::error("Could not play leader animation for action " + key + " because " + playAnimInstance->leaderAnimKey + " animation is undefined.");
			component->stopAction(key, instance);
			return;
		}

		if (!followerAnimations.Contains(playAnimInstance->followerAnimKey)) {
			UDebug::error("Could not play follower animation for action " + key + " because " + playAnimInstance->followerAnimKey + " animation is undefined.");
			component->stopAction(key, instance);
			return;
		}

		ACreature* leader = Cast< ACreature >(playAnimInstance->leader);
		FPlayAnimationData& leaderAnimation = leaderAnimations[playAnimInstance->leaderAnimKey];
		playAnimInstance->leaderAnimSequence = playAnimation(leaderAnimation, leader);
		UCreatureAnimInstance* leaderAnimInstance = leader->getAnimInstance();
		if (leaderAnimInstance) {
			leaderAnimInstance->processPlayAnimStart(leaderAnimation);
		}

		ACreature* follower = Cast< ACreature >(playAnimInstance->follower);
		FPlayAnimationData& followerAnimation = followerAnimations[playAnimInstance->followerAnimKey];
		playAnimInstance->followerAnimSequence = playAnimation(followerAnimation, follower);
		UCreatureAnimInstance* followerAnimInstance = follower->getAnimInstance();
		if (followerAnimInstance) {
			followerAnimInstance->processPlayAnimStart(followerAnimation);
		}

		if (snapToLeader) {
			follower->detachMesh(leader->GetMesh()->GetComponentLocation(), leader->GetMesh()->GetComponentRotation(), true);
		}
		else if (snapToFollower) {
			leader->detachMesh(follower->GetMesh()->GetComponentLocation(), follower->GetMesh()->GetComponentRotation(), true);
		}

		playAnimInstance->animationsStarted = true;
	}

	if (playAnimInstance->leaderAnimFinished && playAnimInstance->followerAnimFinished) {
		processAnimationsFinished(instance, component);
	}

}

void UTwoEntityPlayAnimAction::processLeaderAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityPlayAnimActionInstance* playAnimInstance = Cast< UTwoEntityPlayAnimActionInstance >(instance);
	playAnimInstance->leaderAnimFinished = true;

	if (snapToFollower) {
		ACreature* leader = Cast< ACreature >(playAnimInstance->leader);
		leader->stopSlotAnimation(0.0f);
		leader->reattachMesh(true);
	}
}

void UTwoEntityPlayAnimAction::processFollowerAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityPlayAnimActionInstance* playAnimInstance = Cast< UTwoEntityPlayAnimActionInstance >(instance);
	playAnimInstance->followerAnimFinished = true;

	if (snapToLeader) {
		ACreature* follower = Cast< ACreature >(playAnimInstance->follower);
		follower->stopSlotAnimation(0.0f);
		follower->reattachMesh(true);
	}
}

void UTwoEntityPlayAnimAction::processAnimationsFinished(UActionInstance* instance, UActionComponent* component) {
	component->stopAction(key, instance);
}


void UTwoEntityPlayAnimAction::processActualEnd(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityPlayAnimActionInstance* playAnimInstance = Cast< UTwoEntityPlayAnimActionInstance >(instance);
	
	if (snapToFollower) {
		ACreature* leader = Cast< ACreature >(playAnimInstance->leader);
		leader->stopSlotAnimation(0.0f);
		leader->reattachMesh(true);
	}
	if (snapToLeader) {
		ACreature* follower = Cast< ACreature >(playAnimInstance->follower);
		follower->stopSlotAnimation(0.0f);
		follower->reattachMesh(true);
	}

	Super::processActualEnd(instance, component);
}

void UTwoEntityPlayAnimAction::assignAnimKeys(UActionInstance* instance, UActionComponent* component) {
	UTwoEntityPlayAnimActionInstance* playAnimInstance = Cast< UTwoEntityPlayAnimActionInstance >(instance);
	playAnimInstance->leaderAnimKey = leaderDefaultAnimKey;
	playAnimInstance->followerAnimKey = followerDefaultAnimKey;
}


UClass* UTwoEntityPlayAnimAction::getInstanceClass(UActionComponent* component) {
	return UTwoEntityPlayAnimActionInstance::StaticClass();
}