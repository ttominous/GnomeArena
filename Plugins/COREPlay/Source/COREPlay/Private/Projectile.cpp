#include "Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Creature.h"
#include "ProjectileSpec.h"
#include "WorldDecal.h"
#include "MathTools.h"
#include "Debug.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;
}

void AProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AProjectile, ready, COND_InitialOnly);
	DOREPLIFETIME(AProjectile, direction);
	DOREPLIFETIME(AProjectile, speed);
	DOREPLIFETIME_CONDITION(AProjectile, lifeSpan, COND_InitialOnly);
	DOREPLIFETIME(AProjectile, life);
	DOREPLIFETIME(AProjectile, spec);
}


void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	deSpawn();
	onDeSpawn();
}

void AProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!spawned) {
		if (ready && spawn()) {
			spawned = true;
			onSpawn();
		}
	}
	else {
		update(DeltaTime);
	}
}

bool AProjectile::spawn() {
	if (GetRootComponent() == nullptr) {
		USceneComponent* sceneComponent = NewObject< USceneComponent >(this);
		sceneComponent->RegisterComponent();
		SetRootComponent(sceneComponent);
		SetActorLocation(spawnLocation);
	}
	life = lifeSpan;
	return true;
}

void AProjectile::deSpawn() {

}

void AProjectile::update(float deltaTime) {
	life -= deltaTime;
	if (life <= 0.0f) {
		Destroy();
	}
}

void AProjectile::handleCollision(FHitResult hit) {
	if (collided) {
		return;
	}

	if (spec != nullptr) {
		FDamageDetails damage = spec->damage;
		damage.attacker = attacker;
		damage.attacker.location = GetActorLocation();
		damage.victim = FTargetDetail(hit);

		if (damage.victim.type == ETargetClass::CREATURE) {
			ACreature* creature = damage.victim.getCreature(GetWorld());
			if (creature) {
				creature->processServerDamage(damage);
			}
		}
		else {
			sendWorldCollision(hit);
		}
	}

	if (killOnCollide) {
		life = 0.0f;
	}
	onCollide(hit);
	collided = true;
}

void AProjectile::sendWorldCollision_Implementation(FHitResult hit) {
	handleWorldCollision(hit);
}

void AProjectile::handleWorldCollision(FHitResult hit) {
	if (spec != nullptr) {
		FString effectKey = "Default";

		
		FRotator rotation = hit.Normal.Rotation();
		rotation.Pitch += 90.0f;

		if (spec->hitEffects.Contains(effectKey)) {
			spec->hitEffects[effectKey]->execute(this, hit.Location, rotation, FVector(1.0f));
		}
		if (spec->hitDamageDecals.Contains(effectKey)) {
			AWorldDecal::spawnWorldDecal(GetWorld(), hit.Location, rotation, spec->hitDamageDecals[effectKey]);
		}
	}
}


AProjectile* AProjectile::spawn(UObject* owner, UWorld* world, UProjectileSpec* projectileSpec, FVector location, FRotator direction, float speedModifier) {
	if (projectileSpec->projectileClass == nullptr) {
		UDebug::error("Failed to spawn projectile, " + projectileSpec->displayName + "!No projectile class given.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	AProjectile* projectile = world->SpawnActor<AProjectile>(projectileSpec->projectileClass, location, direction, params);
	if (!projectile) {
		UDebug::error("Failed to spawn projectile, " + projectileSpec->displayName + "!Could not instantiate " + projectileSpec->projectileClass->GetName() + ".");
		return nullptr;
	}

	projectile->spawnLocation = location;
	projectile->SetActorLocation(location);
	projectile->SetActorRotation(direction);
	projectile->attacker = FTargetDetail(owner, location);
	projectile->direction = direction.RotateVector(FVector(1.0f, 0.0f, 0.0f));
	projectile->speed = projectileSpec->speed * speedModifier;
	projectile->lifeSpan = projectileSpec->lifeSpan;
	projectile->spec = projectileSpec;
	projectile->ready = true;
	return projectile;
}