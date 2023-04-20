#include "CollisionTools.h"

FHitResult UCollisionTools::getRayHit(UWorld* world, FVector start, FVector end, ECollisionChannel channel, AActor* ignoredActor) {
	FHitResult result;

	FCollisionQueryParams params;
	params.bTraceComplex = true;
	params.bReturnPhysicalMaterial = false;

    if (ignoredActor != nullptr) {
        params.AddIgnoredActor(ignoredActor);
    }

	world->LineTraceSingleByChannel(result, start, end, channel, params);
	return result;
}

TArray< FHitResult > UCollisionTools::getRayHits(UWorld* world, FVector start, FVector end, ECollisionChannel channel, AActor* ignoredActor) {
    TArray< FHitResult > results;

    FCollisionQueryParams params;
    params.bTraceComplex = true;
    params.bReturnPhysicalMaterial = false;

    if (ignoredActor != nullptr) {
        params.AddIgnoredActor(ignoredActor);
    }

    float dist = FVector::Dist(start, end);
    float distTravelled = 0.0f;
    FVector currentPos = start;

    while (distTravelled < dist) {

        FHitResult result;
        world->LineTraceSingleByChannel(result, currentPos, end, channel, params);

        if (result.bBlockingHit) {
            results.Add(result);
            distTravelled += result.Distance;
            currentPos = result.Location;
            params.AddIgnoredActor(result.GetActor());
        }
        else {
            break;
        }

    }
    return results;
}

TArray< FHitResult > UCollisionTools::getPlayerMouseOverHits(APlayerController* controller, ECollisionChannel channel, float length) {
    float mouseX, mouseY;
    controller->GetMousePosition(mouseX, mouseY);

    FVector start, direction;
    controller->DeprojectScreenPositionToWorld(mouseX,mouseY,start,direction);
  
    return UCollisionTools::getRayHits(controller->GetWorld(), start, start + direction * length, channel, nullptr);
}


ECollisionChannel UCollisionTools::stringToCollisionChannel(FString value) {
	value = value.ToUpper().Replace(TEXT("ECC"),TEXT("")).Replace(TEXT("_"),TEXT(""));
	if (value.Equals("WORLDSTATIC")) {
        return ECollisionChannel::ECC_WorldStatic;
	}
    if (value.Equals("WORLDDYNAMIC")) {
        return ECollisionChannel::ECC_WorldDynamic;
    }
    if (value.Equals("PAWN")) {
        return ECollisionChannel::ECC_Pawn;
    }
    if (value.Equals("VISIBILITY")) {
        return ECollisionChannel::ECC_Visibility;
    }
    if (value.Equals("CAMERA")) {
        return ECollisionChannel::ECC_Camera;
    }
    if (value.Equals("PHYSICSBODY")) {
        return ECollisionChannel::ECC_PhysicsBody;
    }
    if (value.Equals("VEHICLE")) {
        return ECollisionChannel::ECC_Vehicle;
    }
    if (value.Equals("DESTRUCTIBLE")) {
        return ECollisionChannel::ECC_Destructible;
    }
    if (value.Equals("ENGINETRACECHANNEL1")) {
        return ECollisionChannel::ECC_EngineTraceChannel1;
    }
    if (value.Equals("ENGINETRACECHANNEL2")) {
        return ECollisionChannel::ECC_EngineTraceChannel2;
    }
    if (value.Equals("ENGINETRACECHANNEL3")) {
        return ECollisionChannel::ECC_EngineTraceChannel3;
    }
    if (value.Equals("ENGINETRACECHANNEL4")) {
        return ECollisionChannel::ECC_EngineTraceChannel4;
    }
    if (value.Equals("ENGINETRACECHANNEL5")) {
        return ECollisionChannel::ECC_EngineTraceChannel5;
    }
    if (value.Equals("ENGINETRACECHANNEL6")) {
        return ECollisionChannel::ECC_EngineTraceChannel6;
    }
    if (value.Equals("GAMETRACECHANNEL1")) {
        return ECollisionChannel::ECC_GameTraceChannel1;
    }
    if (value.Equals("GAMETRACECHANNEL2")) {
        return ECollisionChannel::ECC_GameTraceChannel2;
    }
    if (value.Equals("GAMETRACECHANNEL3")) {
        return ECollisionChannel::ECC_GameTraceChannel3;
    }
    if (value.Equals("GAMETRACECHANNEL4")) {
        return ECollisionChannel::ECC_GameTraceChannel4;
    }
    if (value.Equals("GAMETRACECHANNEL5")) {
        return ECollisionChannel::ECC_GameTraceChannel5;
    }
    if (value.Equals("GAMETRACECHANNEL6")) {
        return ECollisionChannel::ECC_GameTraceChannel6;
    }
    if (value.Equals("GAMETRACECHANNEL7")) {
        return ECollisionChannel::ECC_GameTraceChannel7;
    }
    if (value.Equals("GAMETRACECHANNEL8")) {
        return ECollisionChannel::ECC_GameTraceChannel8;
    }
    if (value.Equals("GAMETRACECHANNEL9")) {
        return ECollisionChannel::ECC_GameTraceChannel9;
    }
    if (value.Equals("GAMETRACECHANNEL10")) {
        return ECollisionChannel::ECC_GameTraceChannel10;
    }
    if (value.Equals("GAMETRACECHANNEL11")) {
        return ECollisionChannel::ECC_GameTraceChannel11;
    }
    if (value.Equals("GAMETRACECHANNEL12")) {
        return ECollisionChannel::ECC_GameTraceChannel12;
    }
    if (value.Equals("GAMETRACECHANNEL13")) {
        return ECollisionChannel::ECC_GameTraceChannel13;
    }
    if (value.Equals("GAMETRACECHANNEL14")) {
        return ECollisionChannel::ECC_GameTraceChannel14;
    }
    if (value.Equals("GAMETRACECHANNEL15")) {
        return ECollisionChannel::ECC_GameTraceChannel15;
    }
    if (value.Equals("GAMETRACECHANNEL16")) {
        return ECollisionChannel::ECC_GameTraceChannel16;
    }
    if (value.Equals("GAMETRACECHANNEL17")) {
        return ECollisionChannel::ECC_GameTraceChannel17;
    }
    if (value.Equals("GAMETRACECHANNEL18")) {
        return ECollisionChannel::ECC_GameTraceChannel18;
    }
    return ECollisionChannel::ECC_WorldDynamic;
}

ECollisionEnabled::Type UCollisionTools::stringToCollisionEnabled(FString value) {
    value = value.ToUpper();
    if (value.Equals("NOCOLLISION")) {
        return ECollisionEnabled::Type::NoCollision;
    }
    if (value.Equals("QUERYONLY")) {
        return ECollisionEnabled::Type::QueryOnly;
    }
    if (value.Equals("PHYSICSONLY")) {
        return ECollisionEnabled::Type::PhysicsOnly;
    }
    if (value.Equals("QUERYANDPHYSICS")) {
        return ECollisionEnabled::Type::QueryAndPhysics;
    }
    return ECollisionEnabled::Type::NoCollision;
}

bool UCollisionTools::doesActorOverlapBox(AActor* target, FVector boxLocation, FRotator boxRotation, FVector boxExtent) {
    if (target == nullptr) {
        return false;
    }
    FCollisionQueryParams queryParams;
    FCollisionShape collisionShape = FCollisionShape::MakeBox(boxExtent);
    FCollisionResponseParams responseParams;

    TArray< FOverlapResult > overlaps;
    target->GetWorld()->OverlapMultiByChannel(overlaps, boxLocation, boxRotation.Quaternion(), ECollisionChannel::ECC_WorldStatic, collisionShape, queryParams, responseParams);
    for (FOverlapResult& overlap : overlaps) {
        if (overlap.GetActor() == target) {
            return true;
        }
    }
    return false;
}

TArray< FOverlapResult > UCollisionTools::getBoxOverlaps(UWorld* world, FVector location, FRotator rotation, FVector extent, ECollisionChannel channel) {
    FCollisionQueryParams queryParams;
    FCollisionShape collisionShape = FCollisionShape::MakeBox(extent);
    FCollisionResponseParams responseParams;

    TArray< FOverlapResult > overlaps;
    world->OverlapMultiByChannel(overlaps, location, rotation.Quaternion(), channel, collisionShape, queryParams, responseParams);
    return overlaps;
}

TMap< AActor*, FOverlapSet > UCollisionTools::getBoxOverlapsByActor(UWorld* world, FVector location, FRotator rotation, FVector extent, ECollisionChannel channel) {
    TArray< FOverlapResult > overlaps = getBoxOverlaps(world, location, rotation, extent, channel);

    TMap< AActor*, FOverlapSet > results;
    for (FOverlapResult overlap : overlaps) {
        AActor* actor = overlap.GetActor();
        if (results.Contains(actor)) {
            results[actor].overlaps.Add(overlap);
        }
        else {
            FOverlapSet set;
            set.overlaps.Add(overlap);
            results.Add(actor, set);
        }
    }
    return results;
}

TArray< FHitResult > UCollisionTools::getSphereTraceOverlaps(UWorld* world, FVector start, FVector end, float radius, bool returnAllHits, bool traceComplex, ECollisionChannel channel) {
    
    TArray< FHitResult > results;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = traceComplex;

    FCollisionShape collisionShape = FCollisionShape::MakeSphere(radius);
    FCollisionResponseParams responseParams;

    if (returnAllHits) {

        float dist = FVector::Dist(start, end);
        float distTravelled = 0.0f;
        FVector currentPos = start;

        while (distTravelled < dist) {

            TArray< FHitResult > newResults;
            world->SweepMultiByChannel(newResults, currentPos, end, FQuat::Identity, channel, collisionShape, queryParams, responseParams);

            if (newResults.Num() > 0) {
                results.Append(newResults);
                for (FHitResult& hit : newResults) {
                    queryParams.AddIgnoredActor(hit.GetActor());
                }
                distTravelled += newResults[newResults.Num()-1].Distance;
                currentPos = newResults[newResults.Num() - 1].Location;
            }
            else {
                break;
            }
        }

    }
    else {
        world->SweepMultiByChannel(results, start, end, FQuat::Identity, channel, collisionShape, queryParams, responseParams);
    }

    return results;
}

bool UCollisionTools::hasLineOfSight(UWorld* world, AActor* looker, AActor* target, FVector eyeLocation, FVector targetLocation, ECollisionChannel channel) {
   
    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    queryParams.AddIgnoredActor(looker);
    queryParams.AddIgnoredActor(target);

    FCollisionResponseParams responseParams;

    TArray< FHitResult > hits;
    return !world->LineTraceMultiByChannel(hits,eyeLocation,targetLocation,channel,queryParams,responseParams);
}

void UCollisionTools::ignoreCollisions(AActor* actorA, AActor* actorB) {
    if (actorA != nullptr && actorB != nullptr) {
        TArray<UPrimitiveComponent*> componentsA;
        actorA->GetComponents<UPrimitiveComponent>(componentsA);
        for (UPrimitiveComponent* componentA : componentsA) {
            componentA->MoveIgnoreActors.Add(actorB);
        }

        TArray<UPrimitiveComponent*> componentsB;
        actorB->GetComponents<UPrimitiveComponent>(componentsB);
        for (UPrimitiveComponent* componentB : componentsB) {
            componentB->MoveIgnoreActors.Add(actorA);
        }
    }
}

void UCollisionTools::stopIgnoringCollisions(AActor* actorA, AActor* actorB) {
    if (actorA != nullptr && actorB != nullptr) {
        TArray<UPrimitiveComponent*> componentsA;
        actorA->GetComponents<UPrimitiveComponent>(componentsA);
        for (UPrimitiveComponent* componentA : componentsA) {
            componentA->MoveIgnoreActors.Remove(actorB);
        }

        TArray<UPrimitiveComponent*> componentsB;
        actorB->GetComponents<UPrimitiveComponent>(componentsB);
        for (UPrimitiveComponent* componentB : componentsB) {
            componentB->MoveIgnoreActors.Remove(actorA);
        }
    }
}