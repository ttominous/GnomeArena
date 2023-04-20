#include "PortraitStudio.h"
#include "ResourceCache.h"
#include "Creature.h"
#include "SceneObject.h"
#include "SceneObjectSpec.h"
#include "BodyComponent.h"
#include "Debug.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Components/SpotLightComponent.h"

void UPortraitBody::clear() {

}

void UPortraitLivingBody::clear() {
	if (bodyComponent) {
		bodyComponent->destroy();
		bodyComponent->DestroyComponent();
	}
}

void UPortraitStaticBody::clear() {
	for (auto& component : components) {
		if (component.Value) {
			component.Value->DestroyComponent();
		}
	}
	components.Empty();
}




APortraitStudio::APortraitStudio() {
	PrimaryActorTick.bCanEverTick = true;
}

void APortraitStudio::init() {
	physicsCapsule = NewObject<UCapsuleComponent>(this);
	physicsCapsule->RegisterComponent();
	physicsCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(physicsCapsule);
	physicsCapsule->SetRelativeLocation(FVector::ZeroVector);
}

void APortraitStudio::BeginPlay() {
	Super::BeginPlay();
	captureComponent = Cast< UPortraitStudioSceneCaptureComponent2D >(GetComponentByClass(UPortraitStudioSceneCaptureComponent2D::StaticClass()));
	getLight();
	getCameraArm();
}

void APortraitStudio::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	executeNextWorkOrder(DeltaTime);
}

void APortraitStudio::addOrder(UPortrait* order) {
	if (!orders.Contains(order)) {
		orders.Add(order);
	}
}

void APortraitStudio::executeNextWorkOrder(float deltaTime) {

	if (orders.Num() > 0) {

		if (working) {

			if (orders[0]->waitTime > 0.0f) {
				orders[0]->waitTime -= deltaTime;
			}

			if (orders[0]->waitTime <= 0.0f) {
				if (captureComponent) {
					//UTexture2D* resultTexture = NewObject<UTexture2D>(orders[0]);

					UTexture2D* resultTexture = UTexture2D::CreateTransient(captureComponent->TextureTarget->SizeX, captureComponent->TextureTarget->SizeY);
					resultTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
					resultTexture->SRGB = 0;
					resultTexture->AddToRoot();
					resultTexture->UpdateResource();

					captureComponent->TextureTarget->UpdateTexture2D(resultTexture, TSF_RGBA16F);
					resultTexture->UpdateResource();
					orders[0]->material->SetTextureParameterValue("Albedo", resultTexture);
					orders[0]->built = true;
				}

				if (body) {
					body->clear();
					body = nullptr;
				}

				orders.RemoveAt(0);
				working = false;
			}
		}
	}

	if (!working && orders.Num() > 0) {

		GetRootComponent()->SetRelativeLocation(FVector::ZeroVector);
		GetRootComponent()->SetWorldLocation(transform.GetLocation());

		if (!captureComponent) {
			return;
		}
		captureComponent->ShowOnlyActors.Empty();
		captureComponent->ShowOnlyComponents.Empty();
		captureComponent->ShowOnlyActors.Add(this);

		if (orders[0]->target) {
			if (orders[0]->target->IsA(ACreature::StaticClass())) {
				buildFromCreature();
			}
			if (orders[0]->target->IsA(USceneObjectSpec::StaticClass())) {
				buildFromSceneObject();
			}
		}

	}

}


void APortraitStudio::buildFromCreature() {

	ACreature* creature = Cast< ACreature >(orders[0]->target);
	UBodyComponent* creatureBody = UBodyComponent::get(creature);
	if (!creatureBody) {
		return;
	}

	body = NewObject< UPortraitLivingBody >(this);
	UBodyComponent* portraitBody = NewObject< UBodyComponent >(this, creatureBody->GetClass());
	((UPortraitLivingBody*)body)->bodyComponent = portraitBody;
	portraitBody->RegisterComponent();
	creatureBody->copyTo(portraitBody);
	portraitBody->rebuild();

	FVector cameraPosition = GetActorLocation();
	USkeletalMeshComponent* rootMesh = Cast< USkeletalMeshComponent >(portraitBody->getRootComponent());
	if (rootMesh) {
		cameraPosition = rootMesh->GetSocketLocation("Head");
	}

	for (auto& component : portraitBody->components) {
		if (component.Value) {
			UPrimitiveComponent* primitiveComponent = Cast< UPrimitiveComponent > ( component.Value );
			if (primitiveComponent) {
				primitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				primitiveComponent->SetLightingChannels(false, true, false);
			}
		}
	}


	if (getCameraArm()) {
		if (orders[0]->key.EndsWith("closeup")) {
			cameraArm->TargetArmLength = 60.0f;
			cameraPosition.Z += 6.0f;
		}
		else {
			cameraArm->TargetArmLength = 100.0f;
		}
		cameraArm->SetWorldLocation(cameraPosition);
		cameraArm->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
		scaleLight(cameraArm->TargetArmLength * 0.03f);
	}


	working = true;
}

void APortraitStudio::buildFromSceneObject() {

	USceneObjectSpec* spec = Cast< USceneObjectSpec >(orders[0]->target);

	body = NewObject< UPortraitStaticBody >();

	int componentCounter = 1;
	FVector boundsMin = transform.GetLocation();
	FVector boundsMax = transform.GetLocation();

	for (FAssetSpec& asset : spec->assets) {
		USceneComponent* component = nullptr;

		if (asset.type == EAssetType::STATIC_MESH) {
			UStaticMeshComponent* staticMeshComponent = NewObject< UStaticMeshComponent >(this);
			staticMeshComponent->RegisterComponent();
			staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			staticMeshComponent->SetLightingChannels(false, true, false);
			staticMeshComponent->SetStaticMesh(Cast<UStaticMesh>(asset.asset));
			component = staticMeshComponent;

			captureComponent->ShowOnlyComponents.Add(staticMeshComponent);

			component->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
			component->SetRelativeTransform(asset.transform);

			FBox bounds = staticMeshComponent->Bounds.GetBox();
			boundsMin.X = FMath::Min(boundsMin.X, bounds.Min.X);
			boundsMin.Y = FMath::Min(boundsMin.Y, bounds.Min.Y);
			boundsMin.Z = FMath::Min(boundsMin.Z, bounds.Min.Z);
			boundsMax.X = FMath::Max(boundsMax.X, bounds.Max.X);
			boundsMax.Y = FMath::Max(boundsMax.Y, bounds.Max.Y);
			boundsMax.Z = FMath::Max(boundsMax.Z, bounds.Max.Z);
		}
		else {
			continue;
		}


		((UPortraitStaticBody*)body)->components.Add("Component-" + FString::FromInt(componentCounter) , component);
		componentCounter++;

	}

	//captureComponent->ShowOnlyComponents.Add(staticMeshComponent);


	if (getCameraArm()) {
		cameraArm->SetWorldLocation((boundsMin + boundsMax) / 2.0f + spec->iconArmOffset);
		cameraArm->TargetArmLength = spec->iconArmLength;
		cameraArm->SetWorldRotation(FRotator(-25.0f + spec->iconArmRotation.Pitch, 45.0f + spec->iconArmRotation.Yaw, 0.0f + spec->iconArmRotation.Roll));
		scaleLight(cameraArm->TargetArmLength * 0.01f);
	}


	working = true;
}

USpringArmComponent* APortraitStudio::getCameraArm() {
	if (!cameraArm) {
		cameraArm = Cast< USpringArmComponent >(GetComponentByClass(USpringArmComponent::StaticClass()));
	}
	return cameraArm;
}

ULocalLightComponent* APortraitStudio::getLight() {
	if (!light) {
		light = Cast< ULocalLightComponent >(GetComponentByClass(ULocalLightComponent::StaticClass()));
	}
	return light;
}

void APortraitStudio::scaleLight(float range) {
	if (!getLight()) {
		return;
	}
	light->SetIntensity( range );
}