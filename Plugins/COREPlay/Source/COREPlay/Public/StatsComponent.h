#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "StatSet.h"
#include "XMLTools.h"

#include "StatsComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREPLAY_API UStatsComponent : public UActorComponent {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, replicated)
		FStatSet stats;

	UStatsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	static UStatsComponent* get(AActor* actor);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override {
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UStatsComponent, stats);
	}

	UFUNCTION(BlueprintCallable)
	virtual bool hasStat(FString key);

	UFUNCTION(BlueprintCallable)
	virtual int getStatValue(FString key);

	UFUNCTION(BlueprintCallable)
	virtual void setStatValue(FString key, int amount);

	UFUNCTION(BlueprintCallable)
	virtual void addStatValue(FString key, int amount);

	UFUNCTION(BlueprintCallable)
	virtual int getStatMax(FString key);

	UFUNCTION(BlueprintCallable)
	virtual void setStatMax(FString key, int amount);

	UFUNCTION(BlueprintCallable)
	virtual float getStatPerc(FString key);

	UFUNCTION(BlueprintCallable)
	virtual void setStatRegenTime(FString key, float time);


	virtual void writeXML(FXMLBuilderNode& rootNode);
	virtual void readXML(FXmlNode* rootNode);

};