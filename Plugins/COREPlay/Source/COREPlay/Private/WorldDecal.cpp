#include "WorldDecal.h"
#include "SettingsTools.h"
#include "MathTools.h"
#include "Debug.h"


AWorldDecal::AWorldDecal() {
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);
}

void AWorldDecal::BeginPlay() {
	Super::BeginPlay();
}

void AWorldDecal::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (life > 0.0f) {
		life -= DeltaTime;
		if (life <= 0.0f) {
			Destroy();
		}
	}
}

void AWorldDecal::setMaxLife(float MaxLife) {
	life = MaxLife;
	maxLife = MaxLife;
}

AWorldDecal* AWorldDecal::spawnWorldDecal(UWorld* world, FVector location, FRotator rotation, UDecalSpec* decalSpec) {

	if (decalSpec == nullptr || decalSpec->minVisualEffectQualityLevel > USettingsTools::getVisualEffectQuality()) {
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;
	AWorldDecal* decal = world->SpawnActor<AWorldDecal>(AWorldDecal::StaticClass(), location, rotation, params);
	if (!decal) {
		UDebug::error("Failed to spawn AWorldDecal");
		return nullptr;
	}
	decal->SetActorLocation(location);
	decal->SetActorRotation(rotation);

	decal->decalComponent = NewObject< UDecalComponent >(decal);
	decal->decalComponent->RegisterComponent();
	decal->decalComponent->AttachToComponent(decal->RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	decal->decalComponent->SetRelativeLocation(FVector(0, 0, 0));
	decal->setMaxLife(decalSpec->lifeSpan);

	UMaterial* material = nullptr;
	if (decalSpec->materials.Num() > 0) {
		material = decalSpec->materials[FMath::RandRange(0, decalSpec->materials.Num() - 1)];
	}
	decal->decalComponent->SetDecalMaterial(material);
	decal->decalComponent->DecalSize = FVector(decalSpec->size);
	decal->decalComponent->SetRelativeRotation(FRotator(-90, FMath::RandRange(0.0f, 360.0f), 0));

	return decal;
}