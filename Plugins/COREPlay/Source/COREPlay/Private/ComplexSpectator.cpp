#include "ComplexSpectator.h"
#include "Debug.h"


TArray< AStaticCameraPoint* > AStaticCameraPoint::instances;




void AStaticCameraPoint::BeginPlay() {
	Super::BeginPlay();
	if (!instances.Contains(this)) {
		instances.Add(this);
	}
}

void AStaticCameraPoint::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	Super::EndPlay(EndPlayReason);
}



AComplexSpectatorPawn::AComplexSpectatorPawn() {
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);


	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (!camera) {
		UDebug::error("Could not instantiate UCameraComponent!");
		return;
	}
	camera->SetFieldOfView(60.0f);
	camera->SetupAttachment(root);
}

void AComplexSpectatorPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if (!initialized) {
		initialized = true;
		init();
	}
}

void AComplexSpectatorPawn::init() {


}




void ASpectateMapPawn::init() {
	if (AStaticCameraPoint::instances.Num() == 0) {
		initialized = false;
		return;
	}

	AStaticCameraPoint* cameraPoint = AStaticCameraPoint::instances[FMath::RandRange(0, AStaticCameraPoint::instances.Num()-1)];
	SetActorLocation(cameraPoint->GetActorLocation());
	SetActorRotation(cameraPoint->GetActorRotation());
}

ASpectateMapPawn* ASpectateMapPawn::spawn(UWorld* world, FVector location, FRotator direction) {

	FActorSpawnParameters params;
	params.bNoFail = true;

	ASpectateMapPawn* pawn = world->SpawnActor<ASpectateMapPawn>(ASpectateMapPawn::StaticClass(), location, direction, params);
	if (!pawn) {
		UDebug::error("Failed to spawn ASpectateMapPawn!");
		return nullptr;
	}

	pawn->SetActorLocation(location);
	pawn->SetActorRotation(direction);
	return pawn;
}