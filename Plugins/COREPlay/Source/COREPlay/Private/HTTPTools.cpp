#include "HTTPTools.h"
#include "HTTPManager.h"
#include "Debug.h"

FString UHTTPTools::getExternalIP() {
	if (UHTTPManager::instance) {
		return UHTTPManager::instance->getExternalIP();
	}
	return "";
}