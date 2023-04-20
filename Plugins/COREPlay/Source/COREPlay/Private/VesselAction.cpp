#include "VesselAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "SceneObject.h"
#include "CollisionTools.h"
#include "MathTools.h"
#include "Debug.h"





void UVesselAction::processDeferredStart(UActionInstance* instance, UActionComponent* component) {
	UAction::processDeferredStart(instance, component);

	ACreature* creature = Cast< ACreature >(component->GetOwner());
	if (!creature) {
		return;
	}
	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);

	if (!vesselActionInstance->vessel) {
		UDebug::error("Failed to start VesselAction! No vessel.");
		component->stopAction(key, instance);
		return;
	}

	if (animInstance == nullptr) {
		vesselActionInstance->deferredStartComplete = false;
		return;
	}

	ASceneObject* sceneObject = Cast< ASceneObject >(vesselActionInstance->vessel->GetOwner());
	UCollisionTools::ignoreCollisions(creature, sceneObject);

	if (vesselActionInstance->exiting) {
		snapToVesselSlot(instance, component);
		startLoopingAnimation(instance, component);
		vesselActionInstance->exiting = false;
		processInterupt(instance, component, creature->GetActorLocation());
		return;
	}
	else if (vesselActionInstance->entryComplete) {
		snapToVesselSlot(instance, component);
		startLoopingAnimation(instance, component);
		return;
	}

	FTransform accessTransform = vesselActionInstance->vessel->getAccessTransform(vesselActionInstance->slot, vesselActionInstance->accessIndex, false);
	vesselActionInstance->targetLocation = accessTransform.GetLocation();
	vesselActionInstance->targetYaw = accessTransform.GetRotation().Rotator().Yaw;
}


void UVesselAction::processTick(float deltaTime, UActionInstance* instance, UActionComponent* component) {
	Super::processTick(deltaTime, instance, component);

	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	if (!vesselActionInstance->initComplete) {

		ACreature* creature = Cast< ACreature >(component->GetOwner());
		creature->bodyYaw = UMathTools::lerpAngleByDegrees(creature->bodyYaw, vesselActionInstance->targetYaw, deltaTime * 300.0f);

		if (UMathTools::getAngleDiff(creature->bodyYaw, vesselActionInstance->targetYaw, true) <= 2.0f) {
		
			FVesselSlot& slot = vesselActionInstance->vessel->spec->slots[vesselActionInstance->slot];
			FVesselAccess& access = slot.accesses[vesselActionInstance->accessIndex];
			UAnimSequence* sequence = access.entryAnimation;
			if (access.entryAnimation == nullptr) {
				UDebug::error("Failed to start vessel entry animation!");
				return;
			}

			vesselActionInstance->initComplete = true;
			snapToVesselSlot(instance, component);
			vesselActionInstance->animSequenceKey = access.entryAnimation->GetName();
		}

	}
}


bool UVesselAction::processPayload(UActionInstance* instance, UActionComponent* component, FPayload payload) {
	FString type = payload.getString("type");
	if (type.Equals(FPayload::TYPE_ANIM_END)) {
		FString animSequence = payload.getString("animSequence");

		UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
		if (animSequence.Equals(vesselActionInstance->animSequenceKey)) {
			processAnimationFinished(instance, component);
		}
	}
	else if (type.Equals(FPayload::TYPE_ANIM_PING)) {
		FString animSequence = payload.getString("animSequence");

		UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
		if (animSequence.Equals(vesselActionInstance->animSequenceKey)) {
			processAnimationPing(instance, component, payload);
		}
	}
	return true;
}


void UVesselAction::processAnimationPing(UActionInstance* instance, UActionComponent* component, FPayload payload) {

}

void UVesselAction::processAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	
	ACreature* creature = Cast< ACreature >(component->GetOwner());
	UCreatureAnimInstance* animInstance = creature->getAnimInstance();

	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	if (vesselActionInstance->exiting) {
		
		FVector targetLocation = vesselActionInstance->targetLocation + FVector(0.0f, 0.0f, creature->getCapsuleHalfHeight());

		vesselActionInstance->vessel->leaveSlot(vesselActionInstance->slot, creature);
		creature->SetActorLocation(targetLocation, false, nullptr, ETeleportType::ResetPhysics);

		FVector previousMeshPosition = creature->GetMesh()->GetComponentLocation();
		FVector nextMeshPosition = targetLocation + FVector(0, 0, -creature->getCapsuleHalfHeight() - 3.5f);

		animInstance->vesselOffset = previousMeshPosition - nextMeshPosition;

		UCollisionTools::stopIgnoringCollisions(creature, vesselActionInstance->vessel->GetOwner());
		component->stopAction(key, instance);
		animInstance->inVessel = false;
		animInstance->exitingVessel = false;
		animInstance->enteringVessel = true;
		
		creature->bodyYaw = vesselActionInstance->targetYaw;
		creature->reattachMesh(true);
	}
	else if (!vesselActionInstance->entryComplete) {
		vesselActionInstance->entryComplete = true;
		animInstance->enteringVessel = false;
	}
}

void UVesselAction::processInterupt(UActionInstance* instance, UActionComponent* component, FVector targetPosition) {

	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	if (!vesselActionInstance->entryComplete || vesselActionInstance->exiting) {
		return;
	}

	ACreature* creature = Cast< ACreature >(component->GetOwner());

	vesselActionInstance->accessIndex = vesselActionInstance->vessel->getNearestAccess(vesselActionInstance->slot, targetPosition, true);
	if (vesselActionInstance->accessIndex == -1) {
		return;
	}

	FTransform accessTransform = vesselActionInstance->vessel->getAccessTransform(vesselActionInstance->slot, vesselActionInstance->accessIndex, true);
	vesselActionInstance->targetLocation = accessTransform.GetLocation();
	vesselActionInstance->targetYaw = accessTransform.GetRotation().Rotator().Yaw;

	FVesselSlot& slot = vesselActionInstance->vessel->spec->slots[vesselActionInstance->slot];
	FVesselAccess& access = slot.accesses[vesselActionInstance->accessIndex];
	UAnimSequence* sequence = access.exitAnimation;
	if (access.exitAnimation == nullptr) {
		UDebug::error("Failed to start vessel entry animation!");
		return;
	}

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	animInstance->vesselExitAnim = access.exitAnimation;
	vesselActionInstance->animSequenceKey = access.exitAnimation->GetName();
	animInstance->exitingVessel = true;
	vesselActionInstance->exiting = true;

}

UClass* UVesselAction::getInstanceClass(UActionComponent* component) {
	return UVesselActionInstance::StaticClass();
}

void UVesselAction::snapToVesselSlot(UActionInstance* instance, UActionComponent* component) {

	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	ACreature* creature = Cast< ACreature >(component->GetOwner());

	FVesselSlot& slot = vesselActionInstance->vessel->spec->slots[vesselActionInstance->slot];
	FVesselAccess& access = slot.accesses[vesselActionInstance->accessIndex];

	FTransform slotTransform = vesselActionInstance->vessel->getSlotTransform(vesselActionInstance->slot);
	FVector targetPosition = slotTransform.GetLocation() + FVector(0.0f, 0.0f, creature->getCapsuleHalfHeight());

	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	animInstance->vesselEntryAnim = access.entryAnimation;
	animInstance->vesselLoopAnim = slot.loopingAnimation;
	animInstance->inVessel = true;
	animInstance->vesselOffset = targetPosition - creature->GetActorLocation();

	vesselActionInstance->vessel->occupySlot(vesselActionInstance->slot, creature);

	creature->detachMesh(false);
	creature->GetMesh()->SetWorldRotation(slotTransform.GetRotation().Rotator());
	creature->SetActorLocation(targetPosition, false, nullptr, ETeleportType::ResetPhysics);
	creature->updateAnimationsImmediately();

}

void UVesselAction::startLoopingAnimation(UActionInstance* instance, UActionComponent* component) {
	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	ACreature* creature = Cast< ACreature >(component->GetOwner());
	UCreatureAnimInstance* animInstance = creature->getAnimInstance();

	if (animInstance == nullptr) {
		UDebug::error("Failed to start vessel looping animatino. Anim instance is missing.");
		return;
	}

	vesselActionInstance->entryComplete = true;
	animInstance->enteringVessel = false;
	vesselActionInstance->animSequenceKey = "";
}

void UVesselAction::writeXML(FXMLBuilderNode& rootNode, UActionInstance* instance, UActionComponent* component) {
	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);

	rootNode.setAttribute("initComplete", vesselActionInstance->initComplete);
	rootNode.setAttribute("entryComplete", vesselActionInstance->entryComplete);
	rootNode.setAttribute("exiting", vesselActionInstance->exiting);
	rootNode.setAttribute("slot", vesselActionInstance->slot);
	rootNode.setAttribute("accessIndex", (double)vesselActionInstance->accessIndex);
	rootNode.setAttribute("animSequenceKey", vesselActionInstance->animSequenceKey);
	rootNode.setAttribute("targetYaw", vesselActionInstance->targetYaw);
	rootNode.setAttribute("targetLocation", vesselActionInstance->targetLocation);

	FXMLBuilderNode& vesselNode = rootNode.addChild("Vessel");
	FTargetDetail(vesselActionInstance->vessel->GetOwner()).writeXML(vesselNode);
}

void UVesselAction::readXML(FXmlNode* rootNode, UActionInstance* instance, UActionComponent* component) {
	FTargetDetail vesselTarget = FTargetDetail::fromXML(UXMLTools::getFirstChild(rootNode, "Vessel"));
	AActor* vesselActor = vesselTarget.getActor(component->GetWorld());
	if (vesselActor == nullptr) {
		UDebug::error("Could not start vessel action because vessel actor could not be found.");
		return;
	}

	UVesselActionInstance* vesselActionInstance = Cast< UVesselActionInstance >(instance);
	vesselActionInstance->vessel = UVesselComponent::get(vesselActor);
	vesselActionInstance->initComplete = UXMLTools::getAttributeBool(rootNode, "initComplete");
	vesselActionInstance->entryComplete = UXMLTools::getAttributeBool(rootNode, "entryComplete");
	vesselActionInstance->exiting = UXMLTools::getAttributeBool(rootNode, "exiting");
	vesselActionInstance->slot = UXMLTools::getAttributeString(rootNode, "slot");
	vesselActionInstance->accessIndex = UXMLTools::getAttributeInt(rootNode, "accessIndex");
	vesselActionInstance->animSequenceKey = UXMLTools::getAttributeString(rootNode, "animSequenceKey");
	vesselActionInstance->targetYaw = UXMLTools::getAttributeFloat(rootNode, "targetYaw");
	vesselActionInstance->targetLocation = UXMLTools::getAttributeVector(rootNode, "targetLocation");

}