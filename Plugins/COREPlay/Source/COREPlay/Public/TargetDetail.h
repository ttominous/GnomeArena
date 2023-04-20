#pragma once

#include "CoreMinimal.h"
#include "TargetClass.h"

#include "TargetDetail.generated.h"


class ACreature;
class ASceneObject;
class ADropItem;
class FXmlNode;
struct FXMLBuilderNode;

USTRUCT(BlueprintType)
struct COREPLAY_API FTargetDetail {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
		ETargetClass type = ETargetClass::UNKNOWN;

	UPROPERTY(EditAnywhere)
		TArray< FString > address;

	UPROPERTY(EditAnywhere)
		FVector location;

	FTargetDetail() {}

	FTargetDetail(UObject* object, FVector inLocation);
	FTargetDetail(UObject* object);
	FTargetDetail(FVector inLocation);
	FTargetDetail(FHitResult hit);

	FString getAddressString();

	void setObject(UObject* object);
	ACreature* getCreature(UWorld* world);
	ASceneObject* getSceneObject(UWorld* world);
	ADropItem* getDropItem(UWorld* world);
	AActor* getActor(UWorld* world);
	FString getComponentName();
	void setComponentName(FString componentName);
	FName getHitBoneName();
	void setHitBoneName(FString boneName);
	bool isEmpty();
	bool isPopulated();
	bool same(FTargetDetail& otherDetail);

	FVector getCurrentLocation(UWorld* world);
	float getRadius(UWorld* world);
	float getHalfHeight(UWorld* world);

	void writeXML(FXMLBuilderNode& rootNode);
	static FTargetDetail fromXML(FXmlNode* rootNode);

};