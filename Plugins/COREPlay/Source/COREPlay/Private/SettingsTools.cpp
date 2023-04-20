#include "SettingsTools.h"
#include "FileTools.h"
#include "XMLTools.h"
#include "BodyComponent.h"
#include "Debug.h"

int USettingsTools::QUALITY_LOWEST = 0;
int USettingsTools::QUALITY_LOW = 1;
int USettingsTools::QUALITY_MEDIUM = 2;
int USettingsTools::QUALITY_HIGH = 3;
int USettingsTools::QUALITY_ULTRA = 4;

TMap< FString, int > USettingsTools::materialSettings;
UGameUserSettings* USettingsTools::settings = nullptr;
bool USettingsTools::settingsChanged = false;
FIntPoint USettingsTools::currentWindowResolution;

bool USettingsTools::facialAnimationsEnabled = true;
bool USettingsTools::extraPhysicsAnimationsEnabled = true;

void USettingsTools::init() {
	settings = nullptr;
	facialAnimationsEnabled = true;
	extraPhysicsAnimationsEnabled = true;
}

UGameUserSettings* USettingsTools::getEngineSettings() {
	if (settings == nullptr) {
		settings = (GEngine != nullptr) ? GEngine->GameUserSettings : nullptr;
	}
	return settings;
}

void USettingsTools::loadGameSettings(bool apply) {
	FString savePath = UFileTools::getRootSavePath() + "/EngineSettings.txt";
	//UDebug::print("Save Path: " + savePath);

	if (!UFileTools::fileExist(savePath)) {
		saveGameSettings();
		return;
	}
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}

	FXmlNode* rootNode = UXMLTools::getXMLRoot(savePath, false);
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();

	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("GRAPHICS") && apply) {
			if (UXMLTools::hasAttribute(childNode,"UseVSync")) {
				gameUserSettings->SetVSyncEnabled(UXMLTools::getAttributeBool(childNode,"UseVSync"));
			}
			if (UXMLTools::hasAttribute(childNode, "FullscreenMode")) {
				gameUserSettings->SetFullscreenMode((EWindowMode::Type)UXMLTools::getAttributeInt(childNode, "FullscreenMode"));
			}
			if (UXMLTools::hasAttribute(childNode, "ShadowQuality")) {
				gameUserSettings->SetShadowQuality(UXMLTools::getAttributeInt(childNode, "ShadowQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "ShadingQuality")) {
				gameUserSettings->SetShadingQuality(UXMLTools::getAttributeInt(childNode, "ShadingQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "TextureQuality")) {
				gameUserSettings->SetTextureQuality(UXMLTools::getAttributeInt(childNode, "TextureQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "ViewDistanceQuality")) {
				gameUserSettings->SetViewDistanceQuality(UXMLTools::getAttributeInt(childNode, "ViewDistanceQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "FoliageQuality")) {
				gameUserSettings->SetFoliageQuality(UXMLTools::getAttributeInt(childNode, "FoliageQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "PostProcessingQuality")) {
				gameUserSettings->SetPostProcessingQuality(UXMLTools::getAttributeInt(childNode, "PostProcessingQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "VisualEffectQuality")) {
				gameUserSettings->SetVisualEffectQuality(UXMLTools::getAttributeInt(childNode, "VisualEffectQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "AntiAliasingQuality")) {
				gameUserSettings->SetAntiAliasingQuality(UXMLTools::getAttributeInt(childNode, "AntiAliasingQuality"));
			}
			if (UXMLTools::hasAttribute(childNode, "RenderScale")) {
				gameUserSettings->SetResolutionScaleNormalized(UXMLTools::getAttributeFloat(childNode, "RenderScale"));
			}

			FIntPoint windowSize = gameUserSettings->GetScreenResolution();
			if (UXMLTools::hasAttribute(childNode, "WindowSizeX")) {
				windowSize.X = UXMLTools::getAttributeInt(childNode, "WindowSizeX");
			}
			if (UXMLTools::hasAttribute(childNode, "WindowSizeY")) {
				windowSize.Y = UXMLTools::getAttributeInt(childNode, "WindowSizeY");
			}
			TArray< FIntPoint > possibleResolutions = getPossibleScreenResolutionValues();
			if (!possibleResolutions.Contains(windowSize)) {
				windowSize = possibleResolutions[0];
			}
			if (gameUserSettings->GetScreenResolution() != windowSize) {
				gameUserSettings->RequestResolutionChange(windowSize.X, windowSize.Y, gameUserSettings->GetFullscreenMode(),true);
			}
			currentWindowResolution = gameUserSettings->GetScreenResolution();

		}
	}
}

void USettingsTools::saveGameSettings() {
	FString savePath = UFileTools::getRootSavePath() + "/EngineSettings.txt";

	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}

	FXMLBuilder fileBuilder;
	FXMLBuilderNode& graphics = fileBuilder.addNode("Graphics");
	graphics.setAttribute("UseVSync", gameUserSettings->bUseVSync);
	graphics.setAttribute("FullscreenMode", (double)gameUserSettings->GetFullscreenMode());
	graphics.setAttribute("ShadowQuality", (double)gameUserSettings->GetShadowQuality());
	graphics.setAttribute("ShadingQuality", (double)gameUserSettings->GetShadingQuality());
	graphics.setAttribute("TextureQuality", (double)gameUserSettings->GetTextureQuality());
	graphics.setAttribute("ViewDistanceQuality", (double)gameUserSettings->GetViewDistanceQuality());
	graphics.setAttribute("FoliageQuality", (double)gameUserSettings->GetFoliageQuality());
	graphics.setAttribute("PostProcessingQuality", (double)gameUserSettings->GetPostProcessingQuality());
	graphics.setAttribute("VisualEffectQuality", (double)gameUserSettings->GetVisualEffectQuality());
	graphics.setAttribute("AntiAliasingQuality", (double)gameUserSettings->GetAntiAliasingQuality());
	graphics.setAttribute("RenderScale", (double)gameUserSettings->GetResolutionScaleNormalized());

	FIntPoint currentResolution = gameUserSettings->GetScreenResolution();
	graphics.setAttribute("WindowSizeX", (double)currentResolution.X);
	graphics.setAttribute("WindowSizeY", (double)currentResolution.Y);

	UFileTools::saveFile(savePath, fileBuilder.toString());
}


void USettingsTools::setUseVSync(bool useVsync) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetVSyncEnabled(useVsync);
	settingsChanged = true;
}

bool USettingsTools::getUseVSync() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return false;
	}
	return gameUserSettings->IsVSyncEnabled();
}


void USettingsTools::setFullscreenType(EWindowMode::Type fullscreenType) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetFullscreenMode(fullscreenType);
	settingsChanged = true;
}

EWindowMode::Type USettingsTools::getFullscreenType() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return EWindowMode::Type::Fullscreen;
	}
	return gameUserSettings->GetFullscreenMode();
}

void USettingsTools::setShadowQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetShadowQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getShadowQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetShadowQuality();
}

void USettingsTools::setShadingQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetShadingQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getShadingQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetShadingQuality();
}

void USettingsTools::setTextureQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetTextureQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getTextureQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetTextureQuality();
}

void USettingsTools::setViewRangeQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetViewDistanceQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getViewRangeQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetViewDistanceQuality();
}


void USettingsTools::setFoliageQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetFoliageQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getFoliageQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetFoliageQuality();
}


void USettingsTools::setVisualEffectQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetVisualEffectQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getVisualEffectQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetVisualEffectQuality();
}


void USettingsTools::setPostProcessQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetPostProcessingQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getPostProcessQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetPostProcessingQuality();
}


void USettingsTools::setAntiAliasingQuality(int quality) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetAntiAliasingQuality(quality);
	settingsChanged = true;
}

int USettingsTools::getAntiAliasingQuality() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 0;
	}
	return gameUserSettings->GetAntiAliasingQuality();
}


void USettingsTools::setRenderScale(float scale) {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->SetResolutionScaleNormalized(scale);
	settingsChanged = true;
}

float USettingsTools::getRenderScale() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return 1.0f;
	}
	return gameUserSettings->GetResolutionScaleNormalized();
}

FIntPoint USettingsTools::getScreenResolution() {
	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return FIntPoint(1,1);
	}
	return gameUserSettings->GetScreenResolution();
}

void USettingsTools::setScreenResolution(FIntPoint newSize) {
	currentWindowResolution = newSize;
	settingsChanged = true;
}


void USettingsTools::applySettings() {
	saveGameSettings();

	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return;
	}
	gameUserSettings->ApplySettings(true);
	gameUserSettings->RequestResolutionChange(currentWindowResolution.X, currentWindowResolution.Y, gameUserSettings->GetFullscreenMode(), true);
	refreshMaterialSettings();
	UBodyComponent::rebuildAll();
}


void USettingsTools::refreshMaterialSettings() {
	UMaterialParameterCollectionInstance* materialParams = UResourceCache::getMaterialParameterCollectionInstance("GlobalParameters");
	if (materialParams) {
		materialParams->SetScalarParameterValue("ShadowQuality", getShadowQuality());
		materialParams->SetScalarParameterValue("ShadingQuality", getShadingQuality());
		materialParams->SetScalarParameterValue("TextureQuality", getTextureQuality());
		materialParams->SetScalarParameterValue("ViewDistanceQuality", getViewRangeQuality());
		materialParams->SetScalarParameterValue("FoliageQuality", getFoliageQuality());
		materialParams->SetScalarParameterValue("PostProcessingQuality", getPostProcessQuality());
		materialParams->SetScalarParameterValue("VisualEffectQuality", getVisualEffectQuality());
		materialParams->SetScalarParameterValue("AntiAliasingQuality", getAntiAliasingQuality());
	}

	materialSettings.Empty();
	materialSettings.Add("ShadowQuality", getShadowQuality());
	materialSettings.Add("ShadingQuality", getShadingQuality());
	materialSettings.Add("TextureQuality", getTextureQuality());
	materialSettings.Add("ViewDistanceQuality", getViewRangeQuality());
	materialSettings.Add("FoliageQuality", getFoliageQuality());
	materialSettings.Add("PostProcessingQuality", getPostProcessQuality());
	materialSettings.Add("VisualEffectQuality", getVisualEffectQuality());
	materialSettings.Add("AntiAliasingQuality", getAntiAliasingQuality());
}

TMap<FString, FIntPoint> USettingsTools::getPossibleScreenResolutions() {
	TMap<FString, FIntPoint> results;

	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return results;
	}

	TArray<FIntPoint> resolutions = getPossibleScreenResolutionValues();
	for (FIntPoint resolution : resolutions) {
		results.Add(FString::FromInt(resolution.X) + "x" + FString::FromInt(resolution.Y), resolution);
	}
	return results;
}

TArray<FString> USettingsTools::getPossibleScreenResolutionKeys() {
	TArray<FString> results;

	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return results;
	}

	TArray<FIntPoint> resolutions = getPossibleScreenResolutionValues();
	for (FIntPoint resolution : resolutions) {
		results.Add(FString::FromInt(resolution.X) + "x" + FString::FromInt(resolution.Y));
	}
	return results;
}

TArray<FIntPoint> USettingsTools::getPossibleScreenResolutionValues() {
	TArray<FIntPoint> results;

	UGameUserSettings* gameUserSettings = getEngineSettings();
	if (!gameUserSettings) {
		UDebug::error("Could not find UGameUserSettings!");
		return results;
	}

	FIntPoint maxResolution = gameUserSettings->GetDesktopResolution();
	results.Add(maxResolution);

	if (maxResolution.X > 3840 && maxResolution.Y > 2160) {
		results.Add(FIntPoint(3840, 2160));
	}
	if (maxResolution.X > 3440 && maxResolution.Y > 2160) {
		results.Add(FIntPoint(3440, 1440));
	}
	if (maxResolution.X > 2560 && maxResolution.Y > 2160) {
		results.Add(FIntPoint(2560, 1440));
	}
	if (maxResolution.X > 1920 && maxResolution.Y > 1200) {
		results.Add(FIntPoint(1920, 1200));
	}
	if (maxResolution.X > 1920 && maxResolution.Y > 1080) {
		results.Add(FIntPoint(1920, 1080));
	}
	if (maxResolution.X > 1600 && maxResolution.Y > 900) {
		results.Add(FIntPoint(1600, 900));
	}
	if (maxResolution.X > 1366 && maxResolution.Y > 768) {
		results.Add(FIntPoint(1366, 768));
	}
	if (maxResolution.X > 1280 && maxResolution.Y > 1024) {
		results.Add(FIntPoint(1280, 1024));
	}
	if (maxResolution.X > 1024 && maxResolution.Y > 768) {
		results.Add(FIntPoint(1024, 768));
	}
	if (maxResolution.X > 800 && maxResolution.Y > 600) {
		results.Add(FIntPoint(800, 600));
	}
	return results;
}

int USettingsTools::getFramesPerSecond(UWorld* world) {
	if (!world) {
		return 0;
	}
	AComplexGameState* gameState = Cast<AComplexGameState>(world->GetGameState());
	if (gameState) {
		return gameState->getFramesPerSecond();
	}
	return 0;
}