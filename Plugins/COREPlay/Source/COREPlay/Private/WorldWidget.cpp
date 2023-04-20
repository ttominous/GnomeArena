#include "WorldWidget.h"
#include "ResourceCache.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"


AWorldWidget::AWorldWidget() {
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);
}

void AWorldWidget::BeginPlay() {
	Super::BeginPlay();
}

void AWorldWidget::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector position = GetActorLocation();
	position += velocity * DeltaTime;
	SetActorLocation(position);


	if (life > 0.0f) {
		life -= DeltaTime;
		if (life <= 0.0f) {
			Destroy();
		}
		else {

			if (widgetComponent) {
				UBorder* border = UUITools::getBorder(widgetComponent->GetWidget(), "Opacity");
				if (border) {
					FLinearColor color = border->GetContentColorAndOpacity();
					color.A = life / maxLife;
					border->SetContentColorAndOpacity(color);
				}
			}

		}
	}
}

void AWorldWidget::setMaxLife(float MaxLife) {
	life = MaxLife;
	maxLife = MaxLife;
}

AWorldWidget* AWorldWidget::spawnWorldWidget(UWorld* world, FVector location, FString widgetClassKey) {
	FActorSpawnParameters params;
	params.bNoFail = true;
	FRotator rotation = FRotator::ZeroRotator;
	AWorldWidget* worldWidget = world->SpawnActor<AWorldWidget>(AWorldWidget::StaticClass(), location, FRotator::ZeroRotator, params);
	if (!worldWidget) {
		UDebug::error("Failed to spawn AWorldWidget for " + widgetClassKey);
		return nullptr;
	}
	worldWidget->SetActorLocation(location);

	UClass* widgetClass = UResourceCache::getWidgetClass(widgetClassKey);
	if (!widgetClass) {
		worldWidget->Destroy();
		UDebug::error("Failed to find AWorldWidget widget for " + widgetClassKey);
		return nullptr;
	}

	worldWidget->widgetComponent = NewObject< UWidgetComponent >(worldWidget, UWidgetComponent::StaticClass());
	worldWidget->widgetComponent->RegisterComponent();
	worldWidget->widgetComponent->AttachToComponent(worldWidget->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	worldWidget->widgetComponent->SetWidgetClass(widgetClass);
	worldWidget->widgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	return worldWidget;
}

void AWorldWidget::spawnDamageText(UWorld* world, FString text, FVector location, int fontSize, float speed, FColor color) {

	AWorldWidget* worldWidget = spawnWorldWidget(world, location, "DamageText");
	if (!worldWidget) {
		return;
	}

	worldWidget->velocity = UMathTools::randomVectorFromRange(30.0f, 30.0f, 30.0f) * speed;
	worldWidget->setMaxLife(1.0f);

	UUserWidget* widget = worldWidget->widgetComponent->GetWidget();
	UUITools::setTextBlockText(widget, "Text", text);
	UUITools::setTextBlockColor(widget, "Text", FSlateColor(color));


}