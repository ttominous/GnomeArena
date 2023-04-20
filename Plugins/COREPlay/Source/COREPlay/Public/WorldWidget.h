#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"


#include "WorldWidget.generated.h"

UCLASS()
class COREPLAY_API AWorldWidget : public AActor {
	GENERATED_BODY()

public:

	UPROPERTY()
		float life = -1.0f;

	UPROPERTY()
		float maxLife = -1.0f;

	UPROPERTY()
		UWidgetComponent* widgetComponent = nullptr;

	UPROPERTY()
		FVector velocity = FVector::ZeroVector;

	AWorldWidget();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void setMaxLife(float MaxLife);

	static AWorldWidget* spawnWorldWidget(UWorld* world, FVector location, FString widgetClassKey);
	static void spawnDamageText(UWorld* world, FString text, FVector location, int fontSize, float speed, FColor color);
};