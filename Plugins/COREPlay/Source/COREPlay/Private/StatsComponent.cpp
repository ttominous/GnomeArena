#include "StatsComponent.h"
#include "Creature.h"
#include "BodyComponent.h"
#include "Debug.h"



UStatsComponent::UStatsComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACreature* creature = Cast< ACreature >(GetOwner());
	if (creature && creature->dead) {
		return;
	}

	for (FStat& stat : stats.stats) {
		if (stat.regenRate > 0.0001f) {
			stat.regenTime -= DeltaTime;
			if (stat.regenTime <= 0.0f) {
				stat.regenTime = stat.regenRate;
				int newValue = FMath::Min(stat.value + 1, stat.max);
				bool changed = newValue != stat.value;
				stat.value = newValue;

				if (changed && stat.key.Equals("HEALTH")) {
					UBodyComponent* body = UBodyComponent::get(creature);
					if (!body) {
						continue;
					}
					body->setScalarParameter("DamagePerc", 1.0f - getStatPerc("HEALTH"), false);
				}

			}
		}
	}

}


UStatsComponent* UStatsComponent::get(AActor* actor) {
	if (!actor) {
		return nullptr;
	}
	return Cast< UStatsComponent >(actor->GetComponentByClass(UStatsComponent::StaticClass()));
}


bool UStatsComponent::hasStat(FString key) {
	return stats.hasStat(key);
}

int UStatsComponent::getStatValue(FString key) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		return stat.value;
	}
	return 0;
}

void UStatsComponent::setStatValue(FString key, int amount) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		stat.value = FMath::Clamp(amount, 0, stat.max);
	}
}

void UStatsComponent::addStatValue(FString key, int amount) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		stat.value = FMath::Clamp(stat.value + amount, 0, stat.max);
	}
}

int UStatsComponent::getStatMax(FString key) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		return stat.max;
	}
	return 0;
}

void UStatsComponent::setStatMax(FString key, int amount) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		stat.max = FMath::Max(0, amount);
		stat.value = FMath::Clamp(stat.value, 0, stat.max);
	}
}

float UStatsComponent::getStatPerc(FString key) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		if (stat.max > 0) {
			return (float)stat.value / (float)stat.max;
		}
	}
	return 0.0f;
}

void UStatsComponent::setStatRegenTime(FString key, float time) {
	if (stats.hasStat(key)) {
		FStat& stat = stats.get(key);
		stat.regenTime = time;
	}
}

void UStatsComponent::writeXML(FXMLBuilderNode& rootNode) {
	rootNode.setAttribute("type", FString("Stats"));
	for (FStat& stat : stats.stats) {
		FXMLBuilderNode& statNode = rootNode.addChild("Stat");
		statNode.setAttribute("key", stat.key);
		statNode.setAttribute("value", (double)stat.value);
		statNode.setAttribute("max", (double)stat.max);
	}
}

void UStatsComponent::readXML(FXmlNode* rootNode) {
	TArray< FXmlNode* > childNodes = rootNode->GetChildrenNodes();
	for (FXmlNode* childNode : childNodes) {
		FString tag = childNode->GetTag().ToUpper();
		if (tag.Equals("STAT")) {
			FString key = UXMLTools::getAttributeString(childNode, "key");

			if (!stats.hasStat(key)) {
				FStat stat;
				stat.key = key;
				stat.value = UXMLTools::getAttributeInt(childNode, "value");
				stat.max = UXMLTools::getAttributeInt(childNode, "max");
				stats.stats.Add(stat);
			}
			else {
				FStat& stat = stats.get(key);
				stat.value = UXMLTools::getAttributeInt(childNode, "value", stat.value);
				stat.max = UXMLTools::getAttributeInt(childNode, "max", stat.max);
			}

		}
	}
}