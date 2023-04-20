#include "SceneObject.h"
#include "SceneObjectSpec.h"
#include "StatsComponent.h"
#include "VesselComponent.h"
#include "CollisionTools.h"
#include "Debug.h"

#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"



int ASceneObject::lastID = 0;
TArray<ASceneObject*> ASceneObject::instances;

ASceneObject* ASceneObject::getByID(int id) {
	for (ASceneObject* sceneObject : instances) {
		if (sceneObject != nullptr && sceneObject->id == id) {
			return sceneObject;
		}
	}
	return nullptr;
}

ASceneObject::ASceneObject() {
	PrimaryActorTick.bCanEverTick = false;
	this->bReplicates = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
}

void ASceneObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASceneObject, id);
	DOREPLIFETIME(ASceneObject, spec);
}


void ASceneObject::BeginPlay() {
	Super::BeginPlay();
}

void ASceneObject::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (!isGhost) {
		instances.Remove(this);
	}
	Super::EndPlay(EndPlayReason);
}


void ASceneObject::init() {
	if (!isGhost) {
		if (id == -1) {
			id = lastID;
			lastID++;
		}
		if (!instances.Contains(this)) {
			instances.Add(this);
		}
	}
	if (spec == nullptr) {
		return;
	}
	build();
	initialized = true;
}

void ASceneObject::build() {

	USceneComponent* rootComponent = NewObject< USceneComponent >(this);
	SetRootComponent(rootComponent);

	collisionComponent = NewObject<UBoxComponent>(this);
	collisionComponent->RegisterComponent();
	collisionComponent->SetRelativeLocation(FVector(0.0f, 0.0f, spec->collisionBounds.Z));
	collisionComponent->AttachToComponent(rootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	collisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	collisionComponent->SetCollisionProfileName("BlockAllDynamic",true);
	collisionComponent->SetBoxExtent(spec->collisionBounds);

	for (FAssetSpec& asset : spec->assets) {
		USceneComponent* component = nullptr;

		if (asset.type == EAssetType::STATIC_MESH) {
			UStaticMeshComponent* staticMeshComponent = NewObject< UStaticMeshComponent >(this);
			staticMeshComponent->RegisterComponent();
			staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			staticMeshComponent->SetStaticMesh(Cast<UStaticMesh>(asset.asset));
			component = staticMeshComponent;
		}
		else {
			continue;
		}

		component->AttachToComponent(rootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		component->SetRelativeTransform(asset.transform);
	}

	if (!isGhost) {
		if (spec->vesselSpec != nullptr) {
			vesselComponent = NewObject< UVesselComponent >(this);
			vesselComponent->RegisterComponent();
			vesselComponent->spec = spec->vesselSpec;
			vesselComponent->updateAccessibilities();
		}

		if (spec->blockNavmesh) {
			UNavModifierComponent* navModifierComponent = NewObject< UNavModifierComponent >(this);
			navModifierComponent->RegisterComponent();
			navModifierComponent->SetAreaClass(UNavArea_Null::StaticClass());
		}
	}

}

void ASceneObject::makeGhost() {
	UMaterialInterface* ghostMaterial = UResourceCache::getMaterial("GhostValidPlacement");

	TArray<UPrimitiveComponent*> components;
	GetComponents<UPrimitiveComponent>(components);
	for (UPrimitiveComponent* component : components) {
		component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	updateGhostMaterial(true);
}


void ASceneObject::updateGhostMaterial(bool validPlacement) {
	UMaterialInterface* ghostMaterial = UResourceCache::getMaterial(validPlacement ? "GhostValidPlacement" : "GhostInValidPlacement" );

	TArray<UMeshComponent*> meshComponents;
	GetComponents<UMeshComponent>(meshComponents);
	for (UMeshComponent* component : meshComponents) {
		for (int m = 0; m < component->GetNumMaterials(); m++) {
			if (m == 0 && component->GetMaterials()[m] == ghostMaterial) {
				return;
			}
			component->SetMaterial(m, ghostMaterial);
		}
	}
}

bool ASceneObject::isAcceptibleOverlap(FOverlapInfo overlap, bool considerCreatures) {
	AActor* actor = overlap.OverlapInfo.GetActor();
	USceneComponent* component = overlap.OverlapInfo.GetComponent();
	if (actor == nullptr && component != nullptr) {
		actor = component->GetOwner();
	}
	if (!considerCreatures && actor && actor->IsA(ACreature::StaticClass())) {
		return true;
	}
	return isAcceptibleOverlap(actor, component, overlap.OverlapInfo.Location, overlap.OverlapInfo.Item);
}

bool ASceneObject::isAcceptibleOverlap(FOverlapResult overlap, bool considerCreatures) {
	AActor* actor = overlap.GetActor();
	USceneComponent* component = overlap.GetComponent();
	if (actor == nullptr && component != nullptr) {
		actor = component->GetOwner();
	}
	if (!considerCreatures && actor && actor->IsA(ACreature::StaticClass())) {
		return true;
	}
	FVector location = actor != nullptr ? actor->GetActorLocation() : FVector::ZeroVector;
	return isAcceptibleOverlap(actor, component, location, overlap.ItemIndex);
}

bool ASceneObject::isAcceptibleOverlap(AActor* actor, USceneComponent* component, FVector location, int itemIndex) {
	return actor == nullptr || actor != this;
}

bool ASceneObject::isOverlappingUnacceptible() {
	if (!collisionComponent) {
		return false;
	}

	if (isGhost) {
		TArray< FOverlapResult > overlaps = UCollisionTools::getBoxOverlaps(GetWorld(), collisionComponent->GetComponentLocation(), collisionComponent->GetComponentRotation(), spec->collisionBounds, ECollisionChannel::ECC_WorldStatic);
		for (FOverlapResult overlap : overlaps) {
			if (!isAcceptibleOverlap(overlap,true)) {
				return true;
			}
		}
	}
	else {
		TArray< AActor* > actors;
		collisionComponent->GetOverlappingActors(actors);
		for (AActor* actor : actors) {
			TArray< FOverlapInfo > overlaps;
			collisionComponent->GetOverlapsWithActor(actor, overlaps);
			for (FOverlapInfo overlap : overlaps) {
				if (!isAcceptibleOverlap(overlap,true)) {
					return true;
				}
			}
		}
	}
	return false;
}

bool ASceneObject::isMovable() {
	if (vesselComponent) {
		for (auto occupancy : vesselComponent->occupancies) {
			if (occupancy != nullptr) {
				return false;
			}
		}
	}
	return true;
}



void ASceneObject::writeXML(FXMLBuilderNode& rootNode) {
	UStatsComponent* stats = UStatsComponent::get(this);
	if (stats) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		stats->writeXML(componentNode);
	}

	UVesselComponent* vessel = UVesselComponent::get(this);
	if (vessel) {
		FXMLBuilderNode& componentNode = rootNode.addChild("Component");
		vessel->writeXML(componentNode);
	}
}

void ASceneObject::readXML(FXmlNode* rootNode) {
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("COMPONENT")) {
			FString componentType = UXMLTools::getAttributeString(childNode, "type");
			if (componentType.Equals("Stats")) {
				UStatsComponent* stats = UStatsComponent::get(this);
				if (stats) {
					stats->readXML(childNode);
				}
			}
			else if (componentType.Equals("Vessel")) {
				UVesselComponent* vessel = UVesselComponent::get(this);
				if (vessel) {
					vessel->readXML(childNode);
				}
			}
		}
	}
}



ASceneObject* ASceneObject::spawn(UWorld* world, FString specKey, FVector position, float yaw, bool asGhost) {
	USceneObjectSpec* spec = UResourceCache::getSceneObjectSpec(specKey);
	if (spec == nullptr) {
		UDebug::error("Could not find SceneObject Spec '" + specKey + "'");
		return nullptr;
	}
	return spawn(world, spec, position, yaw, asGhost);
}

ASceneObject* ASceneObject::spawn(UWorld* world, USceneObjectSpec* spec, FVector position, float yaw, bool asGhost) {
	
	if (spec == nullptr) {
		UDebug::error("Could not spawn SceneObject, because its Spec was missing.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	UClass* sceneObjectClass = ASceneObject::StaticClass();
	if (spec->sceneObjectClass != nullptr) {
		sceneObjectClass = spec->sceneObjectClass;
	}

	ASceneObject* sceneObject = world->SpawnActor<ASceneObject>(sceneObjectClass, position, FRotator(0.0f, yaw, 0.0f), params);
	if (sceneObject) {
		sceneObject->spec = spec;
		sceneObject->isGhost = asGhost;
		sceneObject->init();
		sceneObject->SetActorLocation(position);
		sceneObject->SetActorRotation(FRotator(0.0f, yaw, 0.0f));
		if (sceneObject->isGhost) {
			sceneObject->makeGhost();
		}
	}
	else {
		UDebug::error("Spawn creature, " + spec->key + " failed!");
	}
	return sceneObject;
}

ASceneObject* ASceneObject::spawn(UWorld* world, UClass* spawnClass, FVector position, float yaw, bool asGhost) {
	if (spawnClass == nullptr) {
		UDebug::error("Could not spawn SceneObject, because its Class was missing.");
		return nullptr;
	}

	FActorSpawnParameters params;
	params.bNoFail = true;

	ASceneObject* sceneObject = world->SpawnActor<ASceneObject>(spawnClass, position, FRotator(0.0f, yaw, 0.0f), params);
	if (sceneObject) {
		sceneObject->isGhost = asGhost;
		sceneObject->init();
		sceneObject->SetActorLocation(position);
		sceneObject->SetActorRotation(FRotator(0.0f, yaw, 0.0f));
		if (sceneObject->isGhost) {
			sceneObject->makeGhost();
		}
	}
	else {
		UDebug::error("Spawn scene object, " + spawnClass->GetName() + " failed!");
	}
	return sceneObject;
}


void ASceneObject::destroyAll() {
	TArray<ASceneObject*> instancesCopy = instances;
	for (ASceneObject* sceneObject : instancesCopy) {
		if (sceneObject) {
			sceneObject->Destroy();
		}
	}
	instances.Empty();
}