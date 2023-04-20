#include "VesselComponent.h"
#include "Creature.h"
#include "CollisionTools.h"
#include "Debug.h"

UVesselComponent::UVesselComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UVesselComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (spec == nullptr) {
		return;
	}
	populateOccupancies();

	if (accessibilityUpdateTimer > 0.0f) {
		accessibilityUpdateTimer -= DeltaTime;
		if (accessibilityUpdateTimer <= 0.0f) {
			updateAccessibilities();
		}
	}

	FRotator rotation = GetOwner()->GetActorRotation();
	FVector location = GetOwner()->GetActorLocation();

	int s = 0;
	for (auto entry : spec->slots) {
		FVesselSlot& slot = entry.Value;

		FRotator slotRotation = rotation + FRotator(0.0f, slot.yaw, 0.0f);
		FVector slotLocation = location + rotation.RotateVector(slot.offset);

		if (spec->drawDebug) {
			UDebug::drawPoint(GetWorld(), slotLocation, 10.0f, FColor::Green, 0.05f);
			UDebug::drawLine(GetWorld(), slotLocation, slotLocation + slotRotation.RotateVector(FVector(0.0f, 25.0f, 0.0f)), FColor::Green, 3.0f, 0.05f);
		}

		int a = 0;
		for (FVesselAccess& access : slot.accesses) {

			FRotator accessRotation = rotation + FRotator(0.0f, access.entryYaw, 0.0f);
			FVector accessLocation = location + rotation.RotateVector(access.offset);

			if (spec->drawDebug) {
				UDebug::drawPoint(GetWorld(), accessLocation, 10.0f, FColor::Yellow, 0.05f);
				UDebug::drawLine(GetWorld(), accessLocation, accessLocation + accessRotation.RotateVector(FVector(0.0f, 25.0f, 0.0f)), FColor::Yellow, 3.0f, 0.05f);
				UDebug::drawBox(GetWorld(), accessLocation, slotRotation.Quaternion(), access.offsetBounds, accessibilities[s].accessibleAccesses[a] ? FColor::Green : FColor::Red, 3.0f, 0.05f);
			}
			a++;
		}
		s++;
	}
}

void UVesselComponent::populateOccupancies() {
	if (spec && occupancies.Num() != spec->slots.Num()) {
		occupancies.Empty();
		occupancies.Init(nullptr, spec->slots.Num());

		accessibilities.Empty();
		for (auto& slot : spec->slots) {
			FVesselSlotAccessibility accessibility;
			accessibility.accessible = false;
			accessibility.accessibleAccesses.Init(false, slot.Value.accesses.Num());
			accessibilities.Add(accessibility);
		}
	}
}

int UVesselComponent::getSlotIndex(FString slotKey) {
	int i = 0;
	for (auto& slot : spec->slots) {
		if (slot.Key.Equals(slotKey)) {
			return i;
		}
		i++;
	}
	return -1;
}

TArray< FString > UVesselComponent::getOpenSlotKeys() {
	populateOccupancies();

	TArray< FString > slotKeys;
	if (spec != nullptr) {
		int i = 0;
		for (auto& slot : spec->slots) {
			if (occupancies[i] == nullptr) {
				slotKeys.Add(slot.Key);
			}
			i++;
		}
	}
	return slotKeys;
}

bool UVesselComponent::isOccupied(FString slotKey) {
	populateOccupancies();
	return spec && spec->slots.Contains(slotKey) && occupancies[getSlotIndex(slotKey)] != nullptr;
}

bool UVesselComponent::isOccupiedBy(FString slotKey, AActor* actor) {
	populateOccupancies();
	return spec && spec->slots.Contains(slotKey) && occupancies[getSlotIndex(slotKey)] == actor;
}

void UVesselComponent::occupySlot(FString slotKey, AActor* actor) {
	if (spec && spec->slots.Contains(slotKey)) {
		populateOccupancies();
		occupancies[getSlotIndex(slotKey)] = actor;
	}
}

void UVesselComponent::leaveSlot(FString slotKey, AActor* actor) {
	if (spec && spec->slots.Contains(slotKey) && occupancies[getSlotIndex(slotKey)] == actor) {
		populateOccupancies();
		occupancies[getSlotIndex(slotKey)] = nullptr;
	}
}

FTransform UVesselComponent::getSlotTransform(FString slotKey) {
	FTransform result;
	if (spec && spec->slots.Contains(slotKey)) {
		FRotator rotation = GetOwner()->GetActorRotation();
		FVector location = GetOwner()->GetActorLocation();

		FVesselSlot& slot = spec->slots[slotKey];
		FRotator slotRotation = rotation + FRotator(0.0f, slot.yaw, 0.0f);
		FVector slotLocation = location + rotation.RotateVector(slot.offset);

		result.SetLocation(slotLocation);
		result.SetRotation(slotRotation.Quaternion());
	}
	return result;
}

UAction* UVesselComponent::getSlotAction(FString slotKey) {
	if (spec && spec->slots.Contains(slotKey)) {
		return spec->slots[slotKey].mainAction;
	}
	return nullptr;
}

FTransform UVesselComponent::getAccessTransform(FString slotKey, int accessIndex, bool asExit) {
	FTransform result;
	if (spec && spec->slots.Contains(slotKey)) {
		FRotator rotation = GetOwner()->GetActorRotation();
		FVector location = GetOwner()->GetActorLocation();

		FVesselSlot& slot = spec->slots[slotKey];
		FRotator slotRotation = rotation + FRotator(0.0f, slot.yaw, 0.0f);
		FVector slotLocation = location + rotation.RotateVector(slot.offset);

		if (accessIndex >= 0 && accessIndex < slot.accesses.Num()) {
			FRotator accessRotation = slotRotation + FRotator(0.0f, asExit ? slot.accesses[accessIndex].exitYaw : slot.accesses[accessIndex].entryYaw, 0.0f);
			FVector accessLocation = slotLocation + slotRotation.RotateVector(asExit ? slot.accesses[accessIndex].entryLocation : slot.accesses[accessIndex].exitLocation);
			result.SetLocation(accessLocation);
			result.SetRotation(accessRotation.Quaternion());
		}
		else {
			result.SetLocation(slotLocation);
			result.SetRotation(slotRotation.Quaternion());
		}

	}
	return result;
}


FString UVesselComponent::getNearestOpenSlotKey(FVector location) {
	TArray< FString > slotKeys = getOpenSlotKeys();
	FString bestSlotKey = "";
	float bestDist = 0.0f;
	
	for (FString slotKey : slotKeys) {

		if (!isSlotAccessible(slotKey)) {
			continue;
		}

		FTransform slotTransform = getSlotTransform(slotKey);
		float dist = FVector::Dist(location,slotTransform.GetLocation());
		if (bestSlotKey.Equals("") || dist < bestDist) {
			bestSlotKey = slotKey;
			bestDist = dist;
		}
	}
	return bestSlotKey;
}

int UVesselComponent::getNearestAccess(FString slotKey, FVector location, bool asExit) {
	if (spec && spec->slots.Contains(slotKey)) {
		FTransform slotTransform = getSlotTransform(slotKey);

		FVesselSlot& slot = spec->slots[slotKey];
		int bestIndex = -1;
		float bestDist = 0.0f;
		int a = 0;
		for (FVesselAccess& access : slot.accesses) {

			if (!isAccessAccessible(slotKey, a)) {
				a++;
				continue;
			}

			FRotator accessRotation = slotTransform.GetRotation().Rotator() + FRotator(0.0f, asExit ? access.exitYaw : access.entryYaw, 0.0f);
			FVector accessLocation = slotTransform.GetLocation() + slotTransform.GetRotation().Rotator().RotateVector(asExit ? access.exitLocation : access.entryLocation);

			float dist = FVector::Dist(accessLocation, location);
			if (bestIndex == -1 || dist < bestDist) {
				bestIndex = a;
				bestDist = dist;
			}
			a++;
		}
		return bestIndex;
	}
	return -1;
}

bool UVesselComponent::isSlotAccessible(FString slotKey) {
	if (spec && spec->slots.Contains(slotKey)) {
		int slotIndex = spec->getSlotIndex(slotKey);
		return accessibilities[slotIndex].accessible;
	}
	return false;
}

bool UVesselComponent::isAccessAccessible(FString slotKey, int accessIndex) {
	if (spec && spec->slots.Contains(slotKey)) {
		int slotIndex = spec->getSlotIndex(slotKey);
		return accessibilities[slotIndex].accessibleAccesses[accessIndex];
	}
	return false;
}

void UVesselComponent::updateAccessibilities() {
	populateOccupancies();

	int s = 0;
	for (auto& slotEntry : spec->slots) {
		accessibilities[s].accessible = false;
		FTransform slotTransform = getSlotTransform(slotEntry.Key);

		int a = 0;
		for (FVesselAccess& access : slotEntry.Value.accesses) {

			FVector accessEntryLocation = slotTransform.GetLocation() + slotTransform.GetRotation().Rotator().RotateVector(access.entryLocation);

			bool overlapsObjects = false;
			TArray< FOverlapResult > overlaps = UCollisionTools::getBoxOverlaps(GetWorld(), accessEntryLocation, slotTransform.GetRotation().Rotator(), access.offsetBounds, ECollisionChannel::ECC_WorldStatic);

			if (GetOwner()->IsA(ASceneObject::StaticClass())) {
				ASceneObject* sceneObject = Cast< ASceneObject >(GetOwner());
				for (FOverlapResult overlap : overlaps) {
					if (!sceneObject->isAcceptibleOverlap(overlap,false)) {
						overlapsObjects = true;
						break;
					}
				}
			}
			else {
				for (FOverlapResult overlap : overlaps) {
					AActor* actor = overlap.GetActor();
					if (actor == nullptr || actor == GetOwner()) {
						continue;
					}
					overlapsObjects = true;
					break;
				}
			}

			if (!overlapsObjects) {
				accessibilities[s].accessible = true;
				accessibilities[s].accessibleAccesses[a] = true;
			}
			else {
				accessibilities[s].accessibleAccesses[a] = false;
			}
			a++;
		}

		s++;
	}

}

void UVesselComponent::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type", FString("Vessel"));
	populateOccupancies();

	FXMLBuilderNode& occupanciesNode = rootNode.addChild("occupancies");
	for (int i = 0; i < occupancies.Num(); i++) {
		FXMLBuilderNode& occupancyNode = occupanciesNode.addChild("occupancy");
		FTargetDetail(occupancies[i]).writeXML(occupancyNode);
	}
}

void UVesselComponent::readXML(FXmlNode* rootNode) {
	populateOccupancies();

	FXmlNode* occupanciesNode = UXMLTools::getFirstChild(rootNode, "occupancies");
	if (occupanciesNode) {
		int i = 0;
		TArray< FXmlNode* > childNodes = occupanciesNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("OCCUPANCY")) {
				occupancies[i] = FTargetDetail::fromXML(childNode).getActor(GetWorld());
				i++;
			}
		}
	}

}

UVesselComponent* UVesselComponent::get(UObject* object) {
	if (object == nullptr || !object->IsA(AActor::StaticClass())) {
		return nullptr;
	}
	return Cast< UVesselComponent >(((AActor*)object)->GetComponentByClass(UVesselComponent::StaticClass()));
}
