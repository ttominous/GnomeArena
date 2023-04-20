#include "RayTracedProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Creature.h"
#include "CollisionTools.h"
#include "MathTools.h"
#include "Debug.h"


bool ARayTracedProjectile::spawn() {
	Super::spawn();
	lastPosition = GetActorLocation();
	return true;
}


void ARayTracedProjectile::update(float deltaTime) {

	FVector currentLocation = GetActorLocation();

	if (!GetWorld()->IsNetMode(ENetMode::NM_Client)) {
		if (!collided) {
			currentLocation += direction * speed * deltaTime;

			FHitResult hit = UCollisionTools::getRayHit(GetWorld(), lastPosition, currentLocation, ECollisionChannel::ECC_PhysicsBody, attacker.getCreature(GetWorld()));
			if (hit.bBlockingHit) {
				handleCollision(hit);
				currentLocation = hit.Location;
				if (debugDraw) {
					UDebug::drawPoint(GetWorld(), hit.Location, 2.0f, FColor::Orange, 3.0f);
				}
			}

			SetActorLocation(currentLocation);
		}
	}

	if (debugDraw) {
		UDebug::drawLine(GetWorld(), lastPosition, currentLocation, FColor::Yellow, 1.0f, 0.05f);
	}

	lastPosition = currentLocation;
	Super::update(deltaTime);
}