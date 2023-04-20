#include "BodyComponent.h"
#include "ResourceCache.h"
#include "BodyPart.h"
#include "DualAxisCreature.h"
#include "CollisionTools.h"
#include "EquipmentComponent.h"
#include "WearableItemSpec.h"
#include "PhysicsMorphComponent.h"
#include "FaceComponent.h"
#include "SettingsTools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

TArray<UBodyComponent*> UBodyComponent::instances;


UBodyComponent::UBodyComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UBodyComponent::BeginPlay() {
	Super::BeginPlay();
	if (!instances.Contains(this)) {
		instances.Add(this);
	}
}

void UBodyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	Super::EndPlay(EndPlayReason);
}

void UBodyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (armed) {
		armedCooldown -= DeltaTime;
		if (armedCooldown <= 0.0f) {
			unarm();
		}
	}
	if (dirty || dirtyShapes || dirtyMaterialParameters) {
		dirtyTimer -= DeltaTime;
		if (dirtyTimer <= 0.0f) {
			if (dirty) {
				rebuild();
			}
			else {
				if (dirtyMaterialParameters) {
					reapplyMaterials();
					dirtyMaterialParameters = false;
				}
				if (dirtyShapes) {
					rebuildShapes();
					reapplyMorphsAndShapes();
				}
			}
		}
	}
}

void UBodyComponent::clear() {
	clearSlots();
	clearParameters();
	clearShapes();
	morphs.Empty();
}

void UBodyComponent::clearSlots() {
	destroy();
	components.Empty();
}

void UBodyComponent::clearParameters() {
	localScalarParameters.Empty();
	localVectorParameters.Empty();
	localTextureParameters.Empty();
	globalScalarParameters.Empty();
	globalVectorParameters.Empty();
	globalTextureParameters.Empty();
}

void UBodyComponent::clearShapes() {
	shapes.Empty();
}

void UBodyComponent::destroy() {
	for (FBodyPartSlot& slot : slots) {
		if (slot.key != rootSlotKey || !GetOwner()->IsA(ACharacter::StaticClass())) {
			if (components.Contains(slot.key)) {
				if (components[slot.key]) {
					components[slot.key]->DestroyComponent();
				}
				components.Remove(slot.key);
			}
		}
	}
}

void UBodyComponent::rebuild() {

	rebuildShapes();

	for (FBodyPartSlot& slot : slots) {
		if (slot.key == rootSlotKey) {
			buildSlot(slot);
		}
	}
	for (FBodyPartSlot& slot : slots) {
		if (slot.key != rootSlotKey) {
			buildSlot(slot);
		}
	}

	rebuildAnimInstance();

	UPhysicsMorphComponent* physicsMorphs = UPhysicsMorphComponent::get(GetOwner());
	if (physicsMorphs) {
		physicsMorphs->rebuild(this);
	}

	refreshHiddenFromOwner();

	dirtyMaterialParameters = false;
	dirty = false;
}

void UBodyComponent::clearAndBuild() {
	animInstance = nullptr;
	rebuild();
}

void UBodyComponent::rebuildAnimInstance(UClass* targetClass) {
	if (animInstance == nullptr || currentInstanceClass != targetClass) {
		USkeletalMeshComponent* rootMeshComponent = Cast< USkeletalMeshComponent >(rootComponent);
		if (rootMeshComponent && targetClass) {
			animInstance = nullptr;
			rootMeshComponent->SetAnimInstanceClass(targetClass);
			animInstance = rootMeshComponent->GetAnimInstance();
			currentInstanceClass = targetClass;

			UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
			if (equipment) {
				equipment->initAnimInstance();
			}

		}
	}

	if (animInstance == nullptr) {
		UDebug::error("rebuildAnimInstance to " + (targetClass ? targetClass->GetName() : "None") + " failed!");
	}
}

void UBodyComponent::rebuildAnimInstance() {
	if (animInstanceClass != nullptr) {
		rebuildAnimInstance(animInstanceClass);
	}
}

void UBodyComponent::rebuildShapes() {
	dirtyShapes = false;
}

void UBodyComponent::destroySlot(FBodyPartSlot& slot) {
	if (slot.key != rootSlotKey) {
		if (components.Contains(slot.key)) {
			if (components[slot.key]) {
				components[slot.key]->DestroyComponent();
			}
			components.Remove(slot.key);
		}
	}
}

UBodyPart* UBodyComponent::getBodyPart(FString slotKey) {
	for (int i = 0; i < slots.Num(); i++) {
		if (slots[i].key.Equals(slotKey)) {
			return getBodyPart(slots[i]);
		}
	}
	return nullptr;
}

UBodyPart* UBodyComponent::getBodyPart(FBodyPartSlot& slot) {
	UEquipmentComponent* equipment = UEquipmentComponent::get(GetOwner());
	if (equipment) {
		UWearableItemSpec* itemSpec = Cast< UWearableItemSpec >( equipment->getEquippedItemSpec(slot.key) );
		if (itemSpec) {
			return itemSpec->bodyPart;
		}
	}
	return UResourceCache::getBodyPart(slot.bodyPartKey);
}

FString UBodyComponent::getFinalAttachSlot(FBodyPartSlot& slot, FBodyPartDetail& details, UBodyPart* bodyPart) {
	FString attachToSlot = details.attachToSlot;
	if (UStringTools::containsIgnoreCase(attachToSlot, "Hand") || UStringTools::containsIgnoreCase(attachToSlot, "Weapon")) {
		ADualAxisCreature* creature = Cast< ADualAxisCreature >(GetOwner());
		if (creature && creature->altHanded) {
			if (UStringTools::containsIgnoreCase(attachToSlot, "Right")) {
				attachToSlot = attachToSlot.Replace(TEXT("Right"), TEXT("Left"));
			}
			else if (UStringTools::containsIgnoreCase(attachToSlot, "Left")) {
				attachToSlot = attachToSlot.Replace(TEXT("Left"), TEXT("Right"));
			}
		}
	}
	return attachToSlot;
}

void UBodyComponent::buildSlot(FBodyPartSlot& slot) {
	destroySlot(slot);

	UBodyPart* bodyPart = getBodyPart(slot);
	bool isRootSlot = slot.key == rootSlotKey;

	if (bodyPart) {

		FBodyPartDetail& details = bodyPart->getDetails(slot.key, this);
		FString attachToSlot = getFinalAttachSlot(slot, details, bodyPart);

		if (details.getType() == EAssetType::SKELETAL_MESH) {

			USkeletalMeshComponent* skeletalMeshComponent = nullptr;
			if (isRootSlot) {
				skeletalMeshComponent = Cast< USkeletalMeshComponent >(getRootComponent());
			}

			if (!skeletalMeshComponent) {
				skeletalMeshComponent = NewObject< USkeletalMeshComponent >(GetOwner());
				skeletalMeshComponent->RegisterComponent();
				skeletalMeshComponent->AttachToComponent(getRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*attachToSlot));
				if (isRootSlot) {
					rootComponent = skeletalMeshComponent;
				}
			}
			else {
				skeletalMeshComponent->EmptyOverrideMaterials();
			}

			if (isRootSlot && GetOwner()->IsA(ACreature::StaticClass())) {
				if (skeletalMeshComponent->GetAttachParent() != nullptr) {
					ACreature* creature = (ACreature*)GetOwner();
					if (!creature->isRagdolling) {
						skeletalMeshComponent->SetRelativeRotation(details.transform.GetRotation());
						skeletalMeshComponent->SetRelativeLocation(FVector(0, 0, -creature->getCapsuleHalfHeight() - 3.5f));
						skeletalMeshComponent->SetRelativeScale3D(details.transform.GetScale3D());
						creature->CacheInitialMeshOffset(skeletalMeshComponent->GetRelativeLocation(), skeletalMeshComponent->GetRelativeRotation());
					}
				}
			}
			else {
				skeletalMeshComponent->SetRelativeRotation(details.transform.GetRotation());
				skeletalMeshComponent->SetRelativeLocation(details.transform.GetLocation());
				skeletalMeshComponent->SetRelativeScale3D(details.transform.GetScale3D());
			}
			skeletalMeshComponent->SetSkeletalMesh(details.getSkeletalMesh());

			if (!isRootSlot && details.useRootSkeletonAnimation && getRootComponent()) {
				skeletalMeshComponent->SetLeaderPoseComponent(Cast< USkeletalMeshComponent >(getRootComponent()));
				skeletalMeshComponent->bUseBoundsFromLeaderPoseComponent = true;
			}
			skeletalMeshComponent->bIncludeComponentLocationIntoBounds = true;
			skeletalMeshComponent->SetReceivesDecals(false);

			applyDefaultPhysics(skeletalMeshComponent,details);
			applyBaseMaterials(details, skeletalMeshComponent);
			applyBaseMorphs(details, skeletalMeshComponent);
			components.Add(slot.key, skeletalMeshComponent);

			skeletalMeshComponent->ComponentTags.Empty();
			for (FString tag : details.tags) {
				skeletalMeshComponent->ComponentTags.Add(FName(*tag));
			}
		}
		else if (details.getType() == EAssetType::STATIC_MESH) {

			UStaticMeshComponent* staticMeshComponent = nullptr;
			if (isRootSlot) {
				staticMeshComponent = Cast< UStaticMeshComponent >(getRootComponent());
			}

			if (!staticMeshComponent) {
				staticMeshComponent = NewObject< UStaticMeshComponent >(GetOwner());
				staticMeshComponent->RegisterComponent();
				staticMeshComponent->AttachToComponent(getRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*attachToSlot));
				staticMeshComponent->SetRelativeLocation(details.transform.GetLocation());
				staticMeshComponent->SetRelativeRotation(details.transform.GetRotation());
				if (isRootSlot) {
					rootComponent = staticMeshComponent;
				}
			}

			staticMeshComponent->SetRelativeScale3D(details.transform.GetScale3D());
			staticMeshComponent->SetStaticMesh(details.getStaticMesh());
			staticMeshComponent->SetReceivesDecals(false);

			applyDefaultPhysics(staticMeshComponent, details);
			applyBaseMaterials(details, staticMeshComponent);
			components.Add(slot.key, staticMeshComponent);

			staticMeshComponent->ComponentTags.Empty();
			for (FString tag : details.tags) {
				staticMeshComponent->ComponentTags.Add(FName(*tag));
			}
		}

	}

	/*UFaceComponent* faceComponent = Cast< UFaceComponent >(GetOwner()->GetComponentByClass(UFaceComponent::StaticClass()));
	if (faceComponent) {
		faceComponent->refreshComponent();
	}*/

}

USceneComponent* UBodyComponent::getRootComponent() {
	if (rootComponent == nullptr) {
		ACharacter* character = Cast< ACharacter >(GetOwner());
		if (character) {
			rootComponent = character->GetMesh();
		}
		else {
			rootComponent = GetOwner()->GetRootComponent();
		}
	}
	return rootComponent;
}

void UBodyComponent::applyBaseMaterials(FBodyPartDetail& details, UMeshComponent* meshComponent) {
	for (FBodyPartMaterial& bodyPartMaterial : details.materials) {
		UMaterialInstance* material = bodyPartMaterial.getMaterialInstance(meshComponent);
		if (material == nullptr) {
			continue;
		}
		meshComponent->SetMaterial(bodyPartMaterial.slotIndex, material);
	}
	applyMaterials(details, meshComponent, true);
}

void UBodyComponent::applyMaterials(FBodyPartDetail& details, UMeshComponent* meshComponent, bool includeTextures) {

	for (FBodyPartMaterial& bodyPartMaterial : details.materials) {

		if (!bodyPartMaterial.instanced() || bodyPartMaterial.slotIndex < 0) {
			continue;
		}
		UMaterialInstanceDynamic* material = Cast< UMaterialInstanceDynamic >(meshComponent->GetMaterial(bodyPartMaterial.slotIndex));
		if (!material) {
			continue;
		}

		for (FString parameterKey : bodyPartMaterial.relevantParameters) {
			FString finalKey = bodyPartMaterial.parameterForwarding.Contains(parameterKey) ? bodyPartMaterial.parameterForwarding[parameterKey] : parameterKey;
			applyMaterialParameter(material, parameterKey, finalKey, includeTextures);
		}

	}

}

void UBodyComponent::applyMaterialParameter(UMaterialInstanceDynamic* material, FString parameterKey, FString finalKey, bool includeTextures) {

	for (auto& param : globalScalarParameters) {
		if (param.key.Equals(parameterKey)) {
			material->SetScalarParameterValue(FName(*finalKey), getFinalScalarParameter(parameterKey, param.value));
			return;
		}
	}
	for (auto& param : localScalarParameters) {
		if (param.key.Equals(parameterKey)) {
			material->SetScalarParameterValue(FName(*finalKey), getFinalScalarParameter(parameterKey, param.value));
			return;
		}
	}

	for (auto& param : globalVectorParameters) {
		if (param.key.Equals(parameterKey)) {
			material->SetVectorParameterValue(FName(*finalKey), getFinalVectorParameter(parameterKey, param.value));
			return;
		}
	}
	for (auto& param : localVectorParameters) {
		if (param.key.Equals(parameterKey)) {
			material->SetVectorParameterValue(FName(*finalKey), getFinalVectorParameter(parameterKey, param.value));
			return;
		}
	}

	if (includeTextures) {
		for (auto& param : globalTextureParameters) {
			if (param.key.Equals(parameterKey)) {
				FString finalValue = getFinalTextureParameter(param.key, param.value);
				material->SetTextureParameterValue(FName(*finalKey), UResourceCache::getTexture(finalValue));
				return;
			}
		}
		for (auto& param : localTextureParameters) {
			if (param.key.Equals(parameterKey)) {
				FString finalValue = getFinalTextureParameter(param.key, param.value);
				material->SetTextureParameterValue(FName(*finalKey), UResourceCache::getTexture(finalValue));
				return;
			}
		}
	}

}

void UBodyComponent::applyBaseMorphs(FBodyPartDetail& details, USkeletalMeshComponent* meshComponent) {
	for (auto& morph : details.morphs) {
		meshComponent->SetMorphTarget(FName(*morph.Key), morph.Value);
	}
	applyMorphsAndShapes(details, meshComponent);
}

void UBodyComponent::applyMorphsAndShapes(FBodyPartDetail& details, USkeletalMeshComponent* meshComponent) {
	for (FString morphKey : details.relevantMorphs) {
		if (morphs.Contains(morphKey)) {
			meshComponent->SetMorphTarget(FName(*morphKey), morphs[morphKey]);
		}
	}
	for (auto& shape : shapes) {
		if (details.relevantMorphs.Contains(shape.key)) {
			meshComponent->SetMorphTarget(FName(*shape.key), shape.value);
		}
	}
}

void UBodyComponent::reapplyMorphsAndShapes() {
	for (auto& slot : slots) {

		if (!components.Contains(slot.key) || components[slot.key] == nullptr) {
			continue;;
		}

		UBodyPart* bodyPart = UResourceCache::getBodyPart(slot.bodyPartKey);
		if (!bodyPart) {
			continue;
		}
		FBodyPartDetail& details = bodyPart->getDetails(slot.key, this);

		USkeletalMeshComponent* meshComponent = Cast< USkeletalMeshComponent >(components[slot.key]);
		if (!meshComponent) {
			continue;
		}
		applyMorphsAndShapes(details, meshComponent);
	}
}

void UBodyComponent::reapplyMaterials() {
	for (auto& slot : slots) {

		if (!components.Contains(slot.key) || components[slot.key] == nullptr) {
			continue;;
		}

		UBodyPart* bodyPart = UResourceCache::getBodyPart(slot.bodyPartKey);
		if (!bodyPart) {
			continue;
		}
		FBodyPartDetail& details = bodyPart->getDetails(slot.key, this);

		UMeshComponent* meshComponent = Cast< UMeshComponent >(components[slot.key]);
		if (!meshComponent) {
			continue;
		}

		applyMaterials(details, meshComponent, true);
	}
}

void UBodyComponent::refreshHiddenFromOwner() {
	ACreature* creature = Cast< ACreature >(GetOwner());
	if (!creature) {
		return;
	}

	for (auto& component : components) {
		if (component.Value != nullptr) {
			UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(component.Value);
			if (primitiveComponent) {
				primitiveComponent->SetOwnerNoSee(creature->hiddenFromOwner);
			}
		}
	}
}

void UBodyComponent::arm() {
	armed = true;
	armedCooldown = 10.0f;
}

void UBodyComponent::unarm() {
	armed = false;
}

void UBodyComponent::setSlot(FString key, FString value) {
	for (int i = 0; i < slots.Num(); i++) {
		if (slots[i].key.Equals(key)) {
			slots[i].bodyPartKey = value;
			return;
		}
	}
	FBodyPartSlot slot;
	slot.key = key;
	slot.bodyPartKey = value;
	slots.Add(slot);
}

FString UBodyComponent::getSlotValue(FString key) {
	for (int i = 0; i < slots.Num(); i++) {
		if (slots[i].key.Equals(key)) {
			return slots[i].bodyPartKey;
		}
	}
	return "";
}

void UBodyComponent::setMorph(FString key, float value, bool applyImmediately) {
	morphs.Add(key, value);

	if (applyImmediately) {
		USkeletalMeshComponent* skeletalMeshComponent = Cast< USkeletalMeshComponent >(getRootComponent());
		if (skeletalMeshComponent) {
			skeletalMeshComponent->SetMorphTarget(FName(*key), value);
		}
	}
}

float UBodyComponent::getMorph(FString key) {
	return morphs.Contains(key) ? morphs[key] : 0.0f;
}

void UBodyComponent::setShape(FString key, float value) {
	for (int i = 0; i < shapes.Num(); i++) {
		if (shapes[i].key.Equals(key)) {
			shapes[i].value = value;
			return;
		}
	}
	FBodyShape shape;
	shape.key = key;
	shape.value = value;
	shapes.Add(shape);
}

float UBodyComponent::getShape(FString key) {
	for (int i = 0; i < shapes.Num(); i++) {
		if (shapes[i].key.Equals(key)) {
			return shapes[i].value;
		}
	}
	return 0.0f;
}

void UBodyComponent::setScalarParameter(FString key, double value, bool global) {
	if (global) {
		for (FBodyScalarParameter& param : globalScalarParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyScalarParameter param;
		param.key = key;
		param.value = value;
		globalScalarParameters.Add(param);
	}
	else {
		dirtyMaterialParameters = true;
		for (FBodyScalarParameter& param : localScalarParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyScalarParameter param;
		param.key = key;
		param.value = value;
		localScalarParameters.Add(param);
	}
}

double UBodyComponent::getScalarParameter(FString key) {
	for (FBodyScalarParameter& param : globalScalarParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	for (FBodyScalarParameter& param : localScalarParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	return 0.0;
}

double UBodyComponent::getFinalScalarParameter(FString key, double currentValue) {
	return currentValue;
}

void UBodyComponent::setVectorParameter(FString key, FVector value, bool global) {
	if (global) {
		for (FBodyVectorParameter& param : globalVectorParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyVectorParameter param;
		param.key = key;
		param.value = value;
		globalVectorParameters.Add(param);
	}
	else {
		dirtyMaterialParameters = true;
		for (FBodyVectorParameter& param : localVectorParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyVectorParameter param;
		param.key = key;
		param.value = value;
		localVectorParameters.Add(param);
	}
}

FVector UBodyComponent::getVectorParameter(FString key) {
	for (FBodyVectorParameter& param : globalVectorParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	for (FBodyVectorParameter& param : localVectorParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	return FVector::ZeroVector;
}

FVector UBodyComponent::getFinalVectorParameter(FString key, FVector currentValue) {
	return currentValue;
}


void UBodyComponent::setColorParameter(FString key, FColor value, bool global) {
	FVector vectorValue = FVector((float)value.R / 255.0f, (float)value.G / 255.0f, (float)value.B / 255.0f);
	setVectorParameter(key, vectorValue, global);
}

FColor UBodyComponent::getColorParameter(FString key) {
	FVector vectorValue = getVectorParameter(key);
	return FColor(255 * vectorValue.X, 255 * vectorValue.Y, 255 * vectorValue.Z);
}

FColor UBodyComponent::getFinalColorParameter(FString key, FColor currentValue) {
	return currentValue;
}

void UBodyComponent::setTextureParameter(FString key, FString value, bool global) {
	if (global) {
		for (FBodyTextureParameter& param : globalTextureParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyTextureParameter param;
		param.key = key;
		param.value = value;
		globalTextureParameters.Add(param);
	}
	else {
		dirtyMaterialParameters = true;
		for (FBodyTextureParameter& param : localTextureParameters) {
			if (param.key.Equals(key)) {
				param.value = value;
				return;
			}
		}
		FBodyTextureParameter param;
		param.key = key;
		param.value = value;
		localTextureParameters.Add(param);
	}
}

FString UBodyComponent::getTextureParameter(FString key) {
	for (FBodyTextureParameter& param : globalTextureParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	for (FBodyTextureParameter& param : localTextureParameters) {
		if (param.key.Equals(key)) {
			return param.value;
		}
	}
	return "";
}

FString UBodyComponent::getFinalTextureParameter(FString key, FString currentValue) {
	return currentValue;
}

void UBodyComponent::arm(AActor* actor) {
	if (!actor) {
		return;
	}
	UBodyComponent* bodyComponent = Cast< UBodyComponent >(actor->GetComponentByClass(UBodyComponent::StaticClass()));
	if (bodyComponent) {
		bodyComponent->arm();
	}
}
void UBodyComponent::unarm(AActor* actor) {
	if (!actor) {
		return;
	}
	UBodyComponent* bodyComponent = Cast< UBodyComponent >(actor->GetComponentByClass(UBodyComponent::StaticClass()));
	if (bodyComponent) {
		bodyComponent->unarm();
	}
}


void UBodyComponent::processPayload(FPayload payload) {
	FString changedValue = payload.getString("ChangedValue", "");
	if (changedValue.Equals("SlotValue")) {
		FString slotKey = payload.getString("Key", "");
		if (slotKey.Len() > 0) {
			FString value = payload.getString("Value", "");
			setSlot(slotKey, value);
			OnRep_RebuildAll();
		}
	}
	else if (changedValue.Equals("Shape")) {
		FString slotKey = payload.getString("Key", "");
		if (slotKey.Len() > 0) {
			float value = payload.getNumeric("Value", 0.0f);
			setShape(slotKey, value);
			OnRep_RebuildShapes();
		}
	}
	else if (changedValue.Equals("ScalarParameter")) {
		FString slotKey = payload.getString("Key", "");
		if (slotKey.Len() > 0) {
			float value = payload.getNumeric("Value", 0.0f);
			bool global = payload.getBool("Global", false);
			setScalarParameter(slotKey, value, global);
			OnRep_RebuildMaterialParameters();
		}
	}
	else if (changedValue.Equals("VectorParameter")) {
		FString slotKey = payload.getString("Key", "");
		if (slotKey.Len() > 0) {
			FVector value = payload.getVector(slotKey, FVector::ZeroVector);
			bool global = payload.getBool("Global", false);
			setVectorParameter(slotKey, value, global);
			OnRep_RebuildMaterialParameters();
		}
	}
	else if (changedValue.Equals("TextureParameter")) {
		FString slotKey = payload.getString("Key", "");
		if (slotKey.Len() > 0) {
			FString value = payload.getString("Value", "");
			bool global = payload.getBool("Global", false);
			setTextureParameter(slotKey, value, global);
			OnRep_RebuildMaterialParameters();
		}
	}
}

void UBodyComponent::setSimulatePhysics(FString boneName, float amount, bool includeSelf) {
	USkeletalMeshComponent* skeletalMeshComponent = Cast< USkeletalMeshComponent >(rootComponent);
	if (!skeletalMeshComponent || (GetOwner()->IsA(ACreature::StaticClass()) && ((ACreature*)GetOwner())->isRagdolling)) {
		return;
	}

	if (amount >= 0.001f) {

		if (skeletalMeshComponent->GetCollisionEnabled() != ECollisionEnabled::PhysicsOnly) {
			skeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			skeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
			skeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		}

		skeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(FName(*boneName), true, includeSelf);
		skeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(FName(*boneName), amount, true, includeSelf);
	}
	else {
		skeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(FName(*boneName), false, includeSelf);
	}
}

void UBodyComponent::applyDefaultPhysics(USceneComponent* sceneComponent, FBodyPartDetail& details) {

	UMeshComponent* meshComponent = Cast<UMeshComponent>(sceneComponent);

	if (!meshComponent || (GetOwner()->IsA(ACreature::StaticClass()) && ((ACreature*)GetOwner())->isRagdolling)) {
		return;
	}
	meshComponent->SetCollisionObjectType(UCollisionTools::stringToCollisionChannel(details.collisionChannel));
	meshComponent->SetCollisionEnabled(UCollisionTools::stringToCollisionEnabled(details.collisionEnabled));

	bool physicsAllowed = USettingsTools::getVisualEffectQuality() >= USettingsTools::QUALITY_HIGH;
	bool enabledPhysics = false;
	if (physicsAllowed) {
		for (FBodyPartPhysics& physics : details.physics) {
			if (physics.simulate) {
				enabledPhysics = true;
				break;
			}
		}
		if (enabledPhysics) {
			meshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		}
	}

	if (meshComponent->IsA(USkeletalMeshComponent::StaticClass())) {
		USkeletalMeshComponent* skeletalMeshComponent = Cast< USkeletalMeshComponent >(meshComponent);
		for (FBodyPartPhysics& physics : details.physics) {
			if (physicsAllowed) {
				skeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(FName(*physics.bone), physics.simulate, physics.includeSelf);
				skeletalMeshComponent->SetAllBodiesBelowPhysicsBlendWeight(FName(*physics.bone), physics.amount, true, physics.includeSelf);
			}
			else {
				skeletalMeshComponent->SetAllBodiesBelowSimulatePhysics(FName(*physics.bone), false, true);
			}
		}
	}


}

void UBodyComponent::applyRootDefaultPhysics() {
	if (!components.Contains(rootSlotKey)) {
		return;
	}
	UBodyPart* bodyPart = getBodyPart(rootSlotKey);
	if (!bodyPart) {
		return;
	}
	FBodyPartDetail& details = bodyPart->getDetails(rootSlotKey, this);
	applyDefaultPhysics(components[rootSlotKey], details);
}

void UBodyComponent::copyTo(UBodyComponent* otherBody) {
	if (!otherBody) {
		return;
	}
	otherBody->rootSlotKey = rootSlotKey;
	otherBody->slots = slots;
	otherBody->animInstanceClass = animInstanceClass;
	otherBody->currentInstanceClass = nullptr;
	otherBody->rootBone = rootBone;
	otherBody->globalScalarParameters = globalScalarParameters;
	otherBody->globalVectorParameters = globalVectorParameters;
	otherBody->globalTextureParameters = globalTextureParameters;
	otherBody->shapes = shapes;
}

USceneComponent* UBodyComponent::getComponent(FString slotKey) {
	if (components.Contains(slotKey)) {
		return components[slotKey];
	}
	return nullptr;
}


void UBodyComponent::OnRep_RebuildAll() {
	dirty = true;
	dirtyTimer = 0.25f;
}

void UBodyComponent::OnRep_RebuildShapes() {
	dirtyShapes = true;
	dirtyTimer = 0.25f;
}

void UBodyComponent::OnRep_RebuildMaterialParameters() {
	dirtyMaterialParameters = true;
	dirtyTimer = 0.25f;
}

void UBodyComponent::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type", FString("Body"));

	if (slots.Num() > 0) {
		FXMLBuilderNode& parametersNode = rootNode.addChild("Slots");
		for (FBodyPartSlot& slot : slots) {
			FXMLBuilderNode& slotNode = parametersNode.addChild("Slot");
			slotNode.setAttribute("key", slot.key);
			slotNode.setAttribute("value", slot.bodyPartKey);
		}
	}

	if (globalScalarParameters.Num() > 0) {
		FXMLBuilderNode& parametersNode = rootNode.addChild("ScalarParameters");
		for (FBodyScalarParameter& parameter : globalScalarParameters) {
			FXMLBuilderNode& parameterNode = parametersNode.addChild("Parameter");
			parameterNode.setAttribute("key", parameter.key);
			parameterNode.setAttribute("value", parameter.value);
		}
	}
	if (globalVectorParameters.Num() > 0) {
		FXMLBuilderNode& parametersNode = rootNode.addChild("VectorParameters");
		for (FBodyVectorParameter& parameter : globalVectorParameters) {
			FXMLBuilderNode& parameterNode = parametersNode.addChild("Parameter");
			parameterNode.setAttribute("key", parameter.key);
			parameterNode.setAttribute("value", parameter.value);
		}
	}
	if (globalTextureParameters.Num() > 0) {
		FXMLBuilderNode& parametersNode = rootNode.addChild("TextureParameters");
		for (FBodyTextureParameter& parameter : globalTextureParameters) {
			FXMLBuilderNode& parameterNode = parametersNode.addChild("Parameter");
			parameterNode.setAttribute("key", parameter.key);
			parameterNode.setAttribute("value", parameter.value);
		}
	}
	if (shapes.Num() > 0) {
		FXMLBuilderNode& shapesNode = rootNode.addChild("Shapes");
		for (FBodyShape& shape : shapes) {
			FXMLBuilderNode& shapeNode = shapesNode.addChild("Shape");
			shapeNode.setAttribute("key", shape.key);
			shapeNode.setAttribute("value", shape.value);
		}
	}

}

void UBodyComponent::readXML(FXmlNode* rootNode) {

	FXmlNode* slotsNode = UXMLTools::getFirstChild(rootNode, "Slots");
	if (slotsNode) {
		TArray< FXmlNode* > childNodes = slotsNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("SLOT")) {
				setSlot(UXMLTools::getAttributeString(childNode, "key"), UXMLTools::getAttributeString(childNode, "value"));
			}
		}
	}

	FXmlNode* scalarParametersNode = UXMLTools::getFirstChild(rootNode, "ScalarParameters");
	if (scalarParametersNode) {
		TArray< FXmlNode* > childNodes = scalarParametersNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("PARAMETER")) {
				setScalarParameter(UXMLTools::getAttributeString(childNode,"key"), UXMLTools::getAttributeFloat(childNode, "value"), true);
			}
		}
	}

	FXmlNode* vectorParametersNode = UXMLTools::getFirstChild(rootNode, "VectorParameters");
	if (vectorParametersNode) {
		TArray< FXmlNode* > childNodes = vectorParametersNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("PARAMETER")) {
				setVectorParameter(UXMLTools::getAttributeString(childNode, "key"), UXMLTools::getAttributeVector(childNode, "value"), true);
			}
		}
	}

	FXmlNode* textureParametersNode = UXMLTools::getFirstChild(rootNode, "TextureParameters");
	if (textureParametersNode) {
		TArray< FXmlNode* > childNodes = textureParametersNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("PARAMETER")) {
				setTextureParameter(UXMLTools::getAttributeString(childNode, "key"), UXMLTools::getAttributeString(childNode, "value"), true);
			}
		}
	}

	FXmlNode* shapesNode = UXMLTools::getFirstChild(rootNode, "Shapes");
	if (shapesNode) {
		TArray< FXmlNode* > childNodes = shapesNode->GetChildrenNodes();
		for (FXmlNode* childNode : childNodes) {
			FString tag = childNode->GetTag().ToUpper();
			if (tag.Equals("SHAPE")) {
				setShape(UXMLTools::getAttributeString(childNode, "key"), UXMLTools::getAttributeFloat(childNode, "value"));
			}
		}
	}

}

UBodyComponent* UBodyComponent::get(AActor* actor) {
	if (actor == nullptr) {
		return nullptr;
	}
	return Cast< UBodyComponent >(actor->GetComponentByClass(UBodyComponent::StaticClass()));
}

void UBodyComponent::rebuildAll() {
	for (UBodyComponent* body : instances) {
		if (body) {
			body->dirty = true;
		}
	}
}