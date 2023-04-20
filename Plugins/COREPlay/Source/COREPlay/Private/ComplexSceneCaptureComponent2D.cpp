#include "ComplexSceneCaptureComponent2D.h"
#include "ViewportTools.h"
#include "Debug.h"

#include "Kismet/KismetRenderingLibrary.h"



TArray< UComplexSceneCaptureComponent2D* > UComplexSceneCaptureComponent2D::instances;

UComplexSceneCaptureComponent2D::UComplexSceneCaptureComponent2D() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UComplexSceneCaptureComponent2D::BeginPlay() {
	Super::BeginPlay();
	refreshViewportSize();
	instances.Add(this);

	bCaptureEveryFrame = true;
	CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SetRelativeLocation(FVector::ZeroVector);
	ShowFlags.Atmosphere = false;

	UCameraComponent* camera = Cast< UCameraComponent >(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	if (camera) {
		FOVAngle = camera->FieldOfView;
	}
}

void UComplexSceneCaptureComponent2D::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	instances.Remove(this);
	Super::EndPlay(EndPlayReason) ;

}
void UComplexSceneCaptureComponent2D::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UCameraComponent* camera = Cast< UCameraComponent >( GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()) );
	if (camera) {
		FOVAngle = camera->FieldOfView;
	}
}

void UComplexSceneCaptureComponent2D::init(FString PassKey, FString materialKey, int ImageSize, float Opacity, FVector Color) {

	passKey = PassKey;
	imageSize = ImageSize;

	renderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), imageSize, imageSize, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false);
	TextureTarget = renderTarget;

	UMaterialInterface* baseMaterial = UResourceCache::getMaterial(materialKey);
	material = UMaterialInstanceDynamic::Create(baseMaterial, GetOwner());
	material->SetTextureParameterValue("Albedo", renderTarget);
	material->SetScalarParameterValue("Opacity", Opacity);
	material->SetVectorParameterValue("Color", Color);


}

void UComplexSceneCaptureComponent2D::addActor(AActor* actor) {
	if (!capturedActors.Contains(actor)) {
		capturedActors.Add(actor);
		ShowOnlyActors.Add(actor);
	}
}

void UComplexSceneCaptureComponent2D::setActor(AActor* actor) {
	if (!capturedActors.Contains(actor) || capturedActors.Num() > 1) {
		clearActors();
		capturedActors.Add(actor);
		ShowOnlyActors.Add(actor);
	}
}

void UComplexSceneCaptureComponent2D::clearActors() {
	capturedActors.Empty();
	ShowOnlyActors.Empty();
}

void UComplexSceneCaptureComponent2D::refreshViewportSize() {

	if (!material) {
		return;
	}

	FIntPoint viewportSize = UViewportTools::getViewportSize(GetOwner());
	float aspectRatio = (float)viewportSize.Y / (float)viewportSize.X;
	material->SetScalarParameterValue("AspectRatio", aspectRatio);
}

void UComplexSceneCaptureComponent2D::refreshAllViewportSizes() {
	for (UComplexSceneCaptureComponent2D* instance : instances) {
		instance->refreshViewportSize();
	}
}

UComplexSceneCaptureComponent2D* UComplexSceneCaptureComponent2D::get(AActor* actor, FString passKey) {
	if (!actor) {
		return nullptr;
	}
	TArray< UComplexSceneCaptureComponent2D* > components;
	actor->GetComponents< UComplexSceneCaptureComponent2D >( components, false);
	for (UComplexSceneCaptureComponent2D* component : components) {
		if (passKey.Equals(component->passKey)) {
			return component;
		}
	}
	return nullptr;
}

TArray< UComplexSceneCaptureComponent2D* > UComplexSceneCaptureComponent2D::getAll(AActor* actor) {
	if (!actor) {
		return TArray< UComplexSceneCaptureComponent2D* >();
	}
	TArray< UComplexSceneCaptureComponent2D* > components;
	actor->GetComponents< UComplexSceneCaptureComponent2D >(components, false);
	return components;
}


UComplexSceneCaptureComponent2D* UComplexSceneCaptureComponent2D::init(AActor* actor, FString passKey, FString materialKey, int ImageSize, float Opacity, FVector Color) {
	if (!actor) {
		return nullptr;
	}

	UComplexSceneCaptureComponent2D* component = UComplexSceneCaptureComponent2D::get(actor, passKey);
	if (component == nullptr) {

		UCameraComponent* camera = Cast< UCameraComponent >(actor->GetComponentByClass(UCameraComponent::StaticClass()));
		if (!camera) {
			return nullptr;
		}

		component = NewObject< UComplexSceneCaptureComponent2D >(actor);
		component->RegisterComponent();
		component->AttachToComponent(camera, FAttachmentTransformRules::SnapToTargetIncludingScale);
		component->bCaptureEveryFrame = true;
		component->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
		component->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		component->FOVAngle = camera->FieldOfView;
		component->SetRelativeLocation(FVector::ZeroVector);
		component->ShowFlags.Atmosphere = false;
	}

	component->init(passKey, materialKey, ImageSize, Opacity, Color);
	component->refreshViewportSize();

	return component;
}

void UComplexSceneCaptureComponent2D::addActor(AActor* actor, FString passKey, AActor* target) {
	UComplexSceneCaptureComponent2D* component = UComplexSceneCaptureComponent2D::get(actor, passKey);
	if (component) {
		component->addActor(target);
	}
}

void UComplexSceneCaptureComponent2D::setActor(AActor* actor, FString passKey, AActor* target) {
	UComplexSceneCaptureComponent2D* component = UComplexSceneCaptureComponent2D::get(actor, passKey);
	if (component) {
		component->setActor(target);
	}
}

void UComplexSceneCaptureComponent2D::clearActors(AActor* actor, FString passKey) {
	UComplexSceneCaptureComponent2D* component = UComplexSceneCaptureComponent2D::get(actor, passKey);
	if (component) {
		component->clearActors();
	}
}