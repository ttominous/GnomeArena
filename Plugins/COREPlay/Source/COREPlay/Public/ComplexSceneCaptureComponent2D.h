#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"

#include "ComplexSceneCaptureComponent2D.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UComplexSceneCaptureComponent2D : public USceneCaptureComponent2D {
	GENERATED_BODY()

public:

	static TArray< UComplexSceneCaptureComponent2D* > instances;

	UPROPERTY()
		FString passKey;

	UPROPERTY()
		int imageSize;

	UPROPERTY()
		UTextureRenderTarget2D* renderTarget = nullptr;

	UPROPERTY()
		UMaterialInstanceDynamic* material = nullptr;

	UPROPERTY()
		TArray< AActor* > capturedActors;

	virtual const AActor* GetViewOwner() const override { return GetOwner(); }


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UComplexSceneCaptureComponent2D();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void init(FString PassKey, FString materialKey, int ImageSize, float Opacity, FVector Color);
	virtual void addActor(AActor* actor);
	virtual void setActor(AActor* actor);
	virtual void clearActors();

	virtual void refreshViewportSize();
	static void refreshAllViewportSizes();

	static UComplexSceneCaptureComponent2D* get(AActor* actor, FString passKey);
	static TArray< UComplexSceneCaptureComponent2D* > getAll(AActor* actor);

	static UComplexSceneCaptureComponent2D* init(AActor* actor, FString passKey, FString materialKey, int ImageSize, float Opacity, FVector Color);
	static void addActor(AActor* actor, FString passKey, AActor* target);
	static void setActor(AActor* actor, FString passKey, AActor* target);
	static void clearActors(AActor* actor, FString passKey);

};