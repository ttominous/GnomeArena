#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

#include "CollisionTools.generated.h"


USTRUCT(BlueprintType)
struct COREPLAY_API FOverlapSet {

	GENERATED_BODY()

public:

	UPROPERTY()
		TArray< FOverlapResult > overlaps;

};

UCLASS()
class COREPLAY_API UCollisionTools : public UObject {

	GENERATED_BODY()

public:

	static FHitResult getRayHit(UWorld* world, FVector start, FVector end, ECollisionChannel channel, AActor* ignoredActor = nullptr);
	static TArray< FHitResult > getRayHits(UWorld* world, FVector start, FVector end, ECollisionChannel channel, AActor* ignoredActor = nullptr);
	static TArray< FHitResult > getPlayerMouseOverHits(APlayerController* controller, ECollisionChannel channel, float length = 99999.0f);

	static ECollisionChannel stringToCollisionChannel(FString value);
	static ECollisionEnabled::Type stringToCollisionEnabled(FString value);

	static bool doesActorOverlapBox(AActor* target, FVector boxLocation, FRotator boxRotation, FVector boxExtent);
	static TArray< FOverlapResult > getBoxOverlaps(UWorld* world, FVector location, FRotator rotation, FVector extent, ECollisionChannel channel = ECollisionChannel::ECC_WorldStatic);
	static TMap< AActor*, FOverlapSet > getBoxOverlapsByActor(UWorld* world, FVector location, FRotator rotation, FVector extent, ECollisionChannel channel = ECollisionChannel::ECC_WorldStatic);

	static TArray< FHitResult > getSphereTraceOverlaps(UWorld* world, FVector start, FVector end, float radius, bool returnAllHits, bool traceComplex, ECollisionChannel channel = ECollisionChannel::ECC_WorldStatic);

	static bool hasLineOfSight(UWorld* world, AActor* looker, AActor* target, FVector eyeLocation, FVector targetLocation, ECollisionChannel channel = ECollisionChannel::ECC_WorldStatic);

	static void ignoreCollisions(AActor* actorA, AActor* actorB);
	static void stopIgnoringCollisions(AActor* actorA, AActor* actorB);

};