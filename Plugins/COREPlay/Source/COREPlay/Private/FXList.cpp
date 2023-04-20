#include "FXList.h"
#include "SettingsTools.h"
#include "MathTools.h"
#include "Debug.h"

#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


void UFXList::execute(AActor* actor, FVector location, float yaw, FVector scale, FName socketName, USceneComponent* component) {
	execute(actor, location, FRotator(0.0f, yaw, 0.0f), scale, socketName, component);
}

void UFXList::execute(AActor* actor, FVector location, FRotator rotation, FVector scale, FName socketName, USceneComponent* component) {
	
	int visualEffectQuality = USettingsTools::getVisualEffectQuality();

	for (FParticleFX& effect : particles) {

		if (FMath::RandRange(0.0f, 1.0f) > effect.chance || visualEffectQuality < effect.minVisualEffectQualityLevel) {
			continue;
		}

		FVector offset = UMathTools::randomVectorFromRange(effect.offsetRangeMin, effect.offsetRangeMax) + effect.offset;
		offset = rotation.RotateVector(offset);

		FVector direction = UMathTools::randomVectorFromRange(effect.rotationMax, effect.rotationMax);
		FRotator finalRotation = FRotator(direction.X, direction.Y, direction.Z) + rotation;

		FVector finalScale = UMathTools::randomVectorFromRange(effect.scaleMin, effect.scaleMax) * scale;

		if (component != nullptr) {
			UNiagaraComponent* niagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(effect.particles, component, socketName, offset, finalRotation, EAttachLocation::KeepRelativeOffset, true, true);
			if (niagaraComponent) {
				niagaraComponent->SetRelativeLocation(offset);
				niagaraComponent->SetRelativeRotation(rotation);
				niagaraComponent->SetRelativeScale3D(finalScale);
			}
		}
		else {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(actor->GetWorld(), effect.particles, location + offset, finalRotation, finalScale, true, true);
		}
	}


	for (FSoundFX& effect : sounds) {

		if (FMath::RandRange(0.0f, 1.0f) > effect.chance) {
			continue;
		}

		if (!effect.sound) {
			continue;
		}

		FVector offset = UMathTools::randomVectorFromRange(effect.offsetRangeMin, effect.offsetRangeMax) + effect.offset;
		offset = rotation.RotateVector(offset);

		FVector direction = UMathTools::randomVectorFromRange(effect.rotationMax, effect.rotationMax);
		FRotator finalRotation = FRotator(direction.X, direction.Y, direction.Z) + rotation;

		float volume = FMath::RandRange(effect.volumeMin, effect.volumeMax);
		float pitch = FMath::RandRange(effect.pitchMin, effect.pitchMax);
		float startTime = FMath::RandRange(effect.startTimeMin, effect.startTimeMax);

		UGameplayStatics::PlaySoundAtLocation(actor->GetWorld(), effect.sound, location, finalRotation, volume, pitch, startTime, nullptr, nullptr, nullptr);
	}
}