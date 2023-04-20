#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameUserSettings.h"


#include "SettingsTools.generated.h"


UCLASS()
class COREPLAY_API USettingsTools : public UObject {

	GENERATED_BODY()

public:

	static int QUALITY_LOWEST;
	static int QUALITY_LOW;
	static int QUALITY_MEDIUM;
	static int QUALITY_HIGH;
	static int QUALITY_ULTRA;

	static TMap< FString, int > materialSettings;
	static UGameUserSettings* settings;
	static bool settingsChanged;
	static FIntPoint currentWindowResolution;

	static bool facialAnimationsEnabled;
	static bool extraPhysicsAnimationsEnabled;

	static void init();
	static UGameUserSettings* getEngineSettings();
	static void loadGameSettings(bool apply);
	static void saveGameSettings();

	static void setUseVSync(bool useVsync);
	static bool getUseVSync();

	static void setFullscreenType(EWindowMode::Type fullscreenType);
	static EWindowMode::Type getFullscreenType();

	static void setShadowQuality(int quality);
	static int getShadowQuality();

	static void setShadingQuality(int quality);
	static int getShadingQuality();

	static void setTextureQuality(int quality);
	static int getTextureQuality();

	static void setViewRangeQuality(int quality);
	static int getViewRangeQuality();

	static void setFoliageQuality(int quality);
	static int getFoliageQuality();

	static void setVisualEffectQuality(int quality);
	static int getVisualEffectQuality();

	static void setPostProcessQuality(int quality);
	static int getPostProcessQuality();

	static void setAntiAliasingQuality(int quality);
	static int getAntiAliasingQuality();

	static void setRenderScale(float scale);
	static float getRenderScale();

	static FIntPoint getScreenResolution();
	static void setScreenResolution(FIntPoint newSize);

	static void applySettings();
	static void refreshMaterialSettings();

	static TMap<FString, FIntPoint> getPossibleScreenResolutions();
	static TArray<FString> getPossibleScreenResolutionKeys();
	static TArray<FIntPoint> getPossibleScreenResolutionValues();

	static int getFramesPerSecond(UWorld* world);

};