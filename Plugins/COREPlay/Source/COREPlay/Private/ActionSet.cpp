#include "ActionSet.h"
#include "Debug.h"

UAction* UActionSet::getAction(FString actionKey) {
	if (actions.Contains(actionKey)) {
		return actions[actionKey];
	}
	return nullptr;
}