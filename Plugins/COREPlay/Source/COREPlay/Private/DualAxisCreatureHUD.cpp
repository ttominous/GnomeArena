#include "DualAxisCreatureHUD.h"
#include "DualAxisCreature.h"
#include "StatsComponent.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "ProjectileWeaponItem.h"
#include "PlayerRecord.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"


void UDualAxisCreatureHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(GetOwningPlayer()->GetPawn());
	if (!creature) {
		UUITools::setVisibility(this, "TakeControl", ESlateVisibility::Hidden);
		UUITools::setVisibility(this, "Dead", ESlateVisibility::Hidden);
		return;
	}

	updateReticle(creature, InDeltaTime);
	updateStats(creature, InDeltaTime);
	updateAmmo(creature, InDeltaTime);
	updateScoreChange(creature, InDeltaTime);

	UUITools::setVisibility(this, "TakeControl", creature->isBot && !creature->dead ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	UUITools::setVisibility(this, "Dead", creature->dead ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

}


void UDualAxisCreatureHUD::updateReticle(ADualAxisCreature* creature, float deltaTime) {

	UBorder* reticleLeft = UUITools::getBorder(this, "ReticleLeft");
	UBorder* reticleRight = UUITools::getBorder(this, "ReticleRight");
	UBorder* reticleUp = UUITools::getBorder(this, "ReticleTop");
	UBorder* reticleDown = UUITools::getBorder(this, "ReticleBottom");
	UBorder* reticleCenter = UUITools::getBorder(this, "ReticleCenter");
	if (!reticleLeft || !reticleRight || !reticleUp || !reticleDown || !reticleCenter) {
		return;
	}


	float opacity = 0.0f;
	if (!creature->isBot) {
		float aimAccuracy = creature->getAimAccuracy();
		targetAimOffset = (1.0f - aimAccuracy) * 250.0f;
		aimOffset = UMathTools::lerpFloatByAmount(aimOffset, targetAimOffset, deltaTime * 500.0f);
		opacity = FMath::Clamp(1.0f - (aimOffset / 250.0f), 0.0f, 1.0f);

		UUITools::setPosition(reticleLeft, FVector2D(-aimOffset, 0.0f));
		UUITools::setPosition(reticleRight, FVector2D(aimOffset, 0.0f));
		UUITools::setPosition(reticleUp, FVector2D(0.0f, -aimOffset));
		UUITools::setPosition(reticleDown, FVector2D(0.0f, aimOffset));
	}

	FSlateColor reticleColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, opacity));
	UUITools::setBrushColor(reticleLeft, reticleColor);
	UUITools::setBrushColor(reticleRight, reticleColor);
	UUITools::setBrushColor(reticleUp, reticleColor);
	UUITools::setBrushColor(reticleDown, reticleColor);
	UUITools::setBrushColor(reticleCenter, reticleColor);

}

void UDualAxisCreatureHUD::updateStats(ADualAxisCreature* creature, float deltaTime) {

	UStatsComponent* stats = UStatsComponent::get(creature);
	if (!stats) {
		return;
	}

	float healthPerc = stats->getStatPerc("HEALTH");
	UUITools::setProgressBarPerc(this, "HealthBar", healthPerc);
	UUITools::setTextBlockText(this, "HealthBarText", FString::FromInt(stats->getStatValue("HEALTH")));

	UUITools::setBrushColor(this, "DyingIndicator", FSlateColor(FLinearColor(1.0f,1.0f,1.0f, (1.0f - healthPerc))));
}

void UDualAxisCreatureHUD::updateAmmo(ADualAxisCreature* creature, float deltaTime) {

	bool defaultToNone = true;

	UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
	if (equipment) {
		UInventoryComponent* inventory = UInventoryComponent::get(creature);
		if (inventory) {
			UItem* item = inventory->getEquippedItem("PrimaryWeapon");
			if (item != nullptr && item->IsA(UProjectileWeaponItem::StaticClass())) {
				defaultToNone = false;

				UProjectileWeaponItem* projectileItem = Cast<UProjectileWeaponItem>(item);
				int clipCount = projectileItem->clipCount;
				UUITools::setTextBlockText(this, "ClipCount", FString::FromInt(projectileItem->clipCount));
				UUITools::setTextBlockColor(this, "ClipCount", clipCount > 0 ? FColor::White : FColor::Red);

				if (projectileItem->ammoItemKey.Len() == 0) {
					UUITools::setTextBlockText(this, "ReserveCount", "--");
					UUITools::setTextBlockColor(this, "ReserveCount", FColor::Silver);
				}
				else {
					int reserveCount = inventory->countItem(projectileItem->ammoItemKey);
					UUITools::setTextBlockText(this, "ReserveCount", FString::FromInt(reserveCount));
					UUITools::setTextBlockColor(this, "ReserveCount", reserveCount > 0 ? FColor::Silver : FColor::Orange);
				}
			}
		}
	}

	if (defaultToNone) {
		UUITools::setTextBlockText(this, "ClipCount", "--");
		UUITools::setTextBlockColor(this, "ClipCount", FColor::White);
		UUITools::setTextBlockText(this, "ReserveCount", "--");
		UUITools::setTextBlockColor(this, "ReserveCount", FColor::Silver);
	}

}

void UDualAxisCreatureHUD::updateScoreChange(ADualAxisCreature* creature, float deltaTime) {
	UPlayerRecord* player = UPlayerRecord::get(creature);
	if (!player) {
		UUITools::setVisibility(this, "ScoreChange", ESlateVisibility::Hidden);
		return;
	}


	if (lastScore != player->score) {
		if(!skipScoreChange){

			int change = player->score - lastScore;
			if (scoreChangeTimer <= 0.001f) {
				scoreChangeBuffer = change;
			}
			else {
				scoreChangeBuffer += change;
			}
			scoreChangeTimer = 3.0f;
		}
		lastScore = player->score;
	}

	if (skipScoreChange) {
		skipScoreChange = false;
	}

	if (scoreChangeTimer > 0.0f) {
		scoreChangeTimer -= deltaTime;

		if (scoreChangeBuffer < 0) {
			UUITools::setTextBlockColor(this, "ScoreChange", FColor(255, 55, 55));
			UUITools::setTextBlockText(this, "ScoreChange", FString::FromInt(scoreChangeBuffer));
		}
		else {
			UUITools::setTextBlockColor(this, "ScoreChange", FColor::White);
			UUITools::setTextBlockText(this, "ScoreChange", "+" + FString::FromInt(scoreChangeBuffer));
		}
		UUITools::setVisibility(this, "ScoreChange", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "ScoreChange", ESlateVisibility::Hidden);
	}

}