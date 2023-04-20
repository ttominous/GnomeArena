#include "ActionComponent.h"
#include "Creature.h"
#include "ResourceCache.h"
#include "StringTools.h"
#include "Debug.h"


UActionComponent::UActionComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


bool UActionComponent::allowMove() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowMove(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowJump() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowJump(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowSprint() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowSprint(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowAim() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowAim(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowSwitchHands() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowSwitchHands(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowTurn() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowTurn(baseActionInstance);
	}
	return true;
}

bool UActionComponent::allowCrouch() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->allowCrouch(baseActionInstance);
	}
	return true;
}

float UActionComponent::getAimAccuracy() {
	if (baseActionInstance != nullptr && baseActionInstance->action != nullptr) {
		return baseActionInstance->action->getAimAccuracy(baseActionInstance);
	}
	return 1.0f;
}

UAction* UActionComponent::getAction(FString actionKey) {
	if (weaponActionSet != nullptr) {
		UAction* action = weaponActionSet->getAction(actionKey);
		if (action) {
			return action;
		}
	}
	if (actionSet != nullptr) {
		return actionSet->getAction(actionKey);
	}
	return nullptr;
}

void UActionComponent::queuePayload(FPayload payload) {
	payloads.Enqueue(payload);
}

bool UActionComponent::processPayload(FPayload payload) {

	FString type = payload.getString("type");

	if (type.Equals(FPayload::TYPE_START_ACTION)) {
		startAction(payload.getString("actionKey"));
		return false;
	}
	else if (type.Equals(FPayload::TYPE_STOP_ACTION)) {
		stopBaseAction(payload.getString("actionKey"));
		return false;
	}
	else if (type.Equals(FPayload::TYPE_PRESS_INPUT)) {
		processInputPress(payload.getString("inputName"));
		return false;
	}
	else if (type.Equals(FPayload::TYPE_RELEASE_INPUT)) {
		processInputRelease(payload.getString("inputName"));
		return false;
	}
	else if (baseActionInstance && baseActionInstance->action) {

		if (GetWorld()->IsNetMode(ENetMode::NM_Client)) {
			ACreature* creature = Cast<ACreature>(GetOwner());
			if (creature && creature->Controller && creature->Controller->IsLocalController()) {
				creature->requestPayload(payload);
			}
		}

		return baseActionInstance->action->processPayload(baseActionInstance, this, payload);
	}

	return true;
}

void UActionComponent::startAction(FString actionKey) {
	startAction(actionKey, FTargetDetail());
}

void UActionComponent::startAction(UAction* action) {
	startAction(action, FTargetDetail());
}

void UActionComponent::startAction(FString actionKey, FTargetDetail target) {
	UAction* action = getAction(actionKey);
	startAction(action, target);
}

void UActionComponent::startAction(UAction* action, FTargetDetail target) {
	startBaseAction(action, target);
}

void UActionComponent::startBaseAction(FString actionKey) {
	UAction* action = getAction(actionKey);
	startBaseAction(action, FTargetDetail());
}

void UActionComponent::startBaseAction(UAction* action) {
	startBaseAction(action, FTargetDetail());
}

void UActionComponent::startBaseAction(FString actionKey, FTargetDetail target) {
	UAction* action = getAction(actionKey);
	startBaseAction(action, target);
}

void UActionComponent::startBaseAction(UAction* action, FTargetDetail target) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		stopBaseAction(baseActionInstance->action->key);
	}

	if (action == nullptr) {
		UDebug::error("Failed to start action because it could not be found.");
		return;
	}

	if (action->bindToLeader) {
		baseActionInstance = action->spawnInstance(this);
		action->processStart(baseActionInstance, this, target);
	}
	else {
		action->processExecute(nullptr, this, target);
	}

}

bool UActionComponent::hasBaseAction(FString actionKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		return baseActionInstance->action->key.Equals(actionKey);
	}
	return false;
}


void UActionComponent::stopAction(FString actionKey, UActionInstance* instance) {
	if (instance == baseActionInstance) {
		stopBaseAction(actionKey);
	}
}

void UActionComponent::stopBaseAction(FString actionKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action && baseActionInstance->action->key.Equals(actionKey)) {
		UActionInstance* endingActionInstance = baseActionInstance;
		baseActionInstance->action->processEnd(baseActionInstance, this);
		if (baseActionInstance == endingActionInstance) {
			baseActionInstance = nullptr;
		}
		//payloads.Empty();
	}
}

void UActionComponent::stopAllActions() {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		stopBaseAction(baseActionInstance->action->key);
	}
}

void UActionComponent::interupt(FVector targetPosition) {
	interuptBaseAction(targetPosition);
}

void UActionComponent::interuptBaseAction(FVector targetPosition) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		UActionInstance* endingActionInstance = baseActionInstance;
		baseActionInstance->action->processInterupt(baseActionInstance, this, targetPosition);
	}
}

void UActionComponent::bindSharedAction(UAction* action, UActionInstance* instance) {
	bindSharedBaseAction(action, instance);
}

void UActionComponent::bindSharedBaseAction(UAction* action, UActionInstance* instance) {
	if (baseActionInstance != nullptr && baseActionInstance->action && baseActionInstance != instance) {
		stopBaseAction(baseActionInstance->action->key);
	}
	baseActionInstance = instance;
}


bool UActionComponent::isBusy() {
	return isBaseActionBusy();
}

bool UActionComponent::isBaseActionBusy() {
	return baseActionInstance != nullptr && baseActionInstance->action != nullptr;
}

bool UActionComponent::canCasuallyInterupt() {
	return canCasuallyInteruptBaseAction();
}

bool UActionComponent::canCasuallyInteruptBaseAction() {
	if (baseActionInstance == nullptr || baseActionInstance->action == nullptr) {
		return true;
	}
	return baseActionInstance->action->allowCasualInterupts;
}

float UActionComponent::getCompletionPerc() {
	if (baseActionInstance != nullptr) {
		return getCompletionPerc(baseActionInstance, baseActionInstance->action);
	}
	return 0.0f;
}

float UActionComponent::getCompletionPerc(UAction* action) {
	if (baseActionInstance != nullptr && baseActionInstance->action == action) {
		return getCompletionPerc(baseActionInstance, baseActionInstance->action);
	}
	return 0.0f;
}

float UActionComponent::getCompletionPerc(UActionInstance* instance, UAction* action) {
	if (action == nullptr) {
		return 0.0f;
	}
	return action->getCompletionPerc(instance, this);
}

void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	tickActions(DeltaTime);

	while (!payloads.IsEmpty()) {
		FPayload payload;
		payloads.Dequeue(payload);

		if (!processPayload(payload)) {
			break;
		}
	}
}

void UActionComponent::tickActions(float deltaTime) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		if (!baseActionInstance->deferredStartComplete) {
			baseActionInstance->deferredStartComplete = true;
			baseActionInstance->action->processDeferredStart(baseActionInstance, this);
		}
		else if (baseActionInstance->action->ticks) {
			baseActionInstance->action->processTick(deltaTime, baseActionInstance, this);
		}
	}
}

void UActionComponent::startProcessInputPress(FString inputKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action && baseActionInstance->action->isRelevantInput(inputKey)) {
		sendKeyPress(inputKey);
	}
}

void UActionComponent::processInputPress(FString inputKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		baseActionInstance->action->processInputPress(inputKey,baseActionInstance, this);
	}
}

void UActionComponent::sendKeyPress(FString inputKey) {
	ACreature* creature = Cast<ACreature>(GetOwner());
	if (creature && creature->Controller && creature->Controller->IsLocalController()) {
		creature->requestPayload(FPayload::pressInput(inputKey));
	}
}

void UActionComponent::startProcessInputRelease(FString inputKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action && baseActionInstance->action->isRelevantInput(inputKey)) {
		sendKeyRelease(inputKey);
	}
}

void UActionComponent::processInputRelease(FString inputKey) {
	if (baseActionInstance != nullptr && baseActionInstance->action) {
		baseActionInstance->action->processInputRelease(inputKey,baseActionInstance, this);
	}
}

void UActionComponent::sendKeyRelease(FString inputKey) {
	ACreature* creature = Cast<ACreature>(GetOwner());
	if (creature && creature->Controller && creature->Controller->IsLocalController()) {
		creature->requestPayload(FPayload::releaseInput(inputKey));
	}
}

void UActionComponent::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type", FString("Action"));
	if (baseActionInstance != nullptr && baseActionInstance->action && baseActionInstance->action->persistInSave) {
		FXMLBuilderNode& actionNode = rootNode.addChild("BaseAction");
		actionNode.setAttribute("actionKey", baseActionInstance->action->key);
		actionNode.setAttribute("actionPath", UStringTools::getAssetPathString(baseActionInstance->action));
		baseActionInstance->action->writeXML(actionNode, baseActionInstance, this);
	}
}

void UActionComponent::readXML(FXmlNode* rootNode) {
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("BASEACTION")) {
			FString actionKey = UXMLTools::getAttributeString(childNode,"actionKey");
			UAction* action = getAction(actionKey);
			if (action == nullptr) {
				FString actionPath = UXMLTools::getAttributeString(childNode, "actionPath");
				action = UResourceCache::getAction(actionPath);
			}

			if (action) {
				startBaseAction(action);
				action->readXML(childNode, baseActionInstance, this);
			}
		}
	}
}

UActionComponent* UActionComponent::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}
	return Cast< UActionComponent >(actor->GetComponentByClass(UActionComponent::StaticClass()));
}