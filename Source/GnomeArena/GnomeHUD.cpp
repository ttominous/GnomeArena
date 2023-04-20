#include "GnomeHUD.h"
#include "DualAxisCreature.h"
#include "StatsComponent.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "ComplexGameState.h"
#include "ProjectileWeaponItemSpec.h"
#include "ProjectileWeaponItem.h"
#include "PlayerRecord.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"


void UGnomeHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	updateDeathAlerts(InDeltaTime);
	updateScoreBoard(InDeltaTime);

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(GetOwningPlayer()->GetPawn());
	if (creature == nullptr) {
		return;
	}

	updateReticle(creature, InDeltaTime);
	updateStats(creature, InDeltaTime);
	updateAmmo(creature, InDeltaTime);
	updateScoreChange(creature, InDeltaTime);

	UUITools::setVisibility(this, "TakeControl", creature->isBot && !creature->dead ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (creature->dead) {
		UUITools::setVisibility(this, "Dead", ESlateVisibility::Visible);

		UUITools::setTextBlockText(this, "RespawnTime", FString::FromInt(FGenericPlatformMath::CeilToInt(creature->despawnTime)));
		UUITools::setVisibility(this, "Respawn", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "Dead", ESlateVisibility::Hidden);
		UUITools::setVisibility(this, "Respawn", ESlateVisibility::Hidden);
	}
}


void UGnomeHUD::updateReticle(ADualAxisCreature* creature, float deltaTime) {

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

void UGnomeHUD::updateStats(ADualAxisCreature* creature, float deltaTime) {

	UStatsComponent* stats = UStatsComponent::get(creature);
	if (!stats) {
		return;
	}

	float healthPerc = stats->getStatPerc("HEALTH");
	UUITools::setProgressBarPerc(this, "HealthBar", healthPerc);
	UUITools::setTextBlockText(this, "HealthBarText", FString::FromInt(stats->getStatValue("HEALTH")));

	UUITools::setBrushColor(this, "DyingIndicator", FSlateColor(FLinearColor(1.0f,1.0f,1.0f, (1.0f - healthPerc))));
}

void UGnomeHUD::updateAmmo(ADualAxisCreature* creature, float deltaTime) {

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
					UUITools::setTextBlockColor(this, "ReserveCount", reserveCount > 0 ? FLinearColor(0.25f,0.25f,0.25f,1.0f) : FColor::Orange);
				}
			}
		}
	}

	if (defaultToNone) {
		UUITools::setTextBlockText(this, "ClipCount", "--");
		UUITools::setTextBlockColor(this, "ClipCount", FColor::White);
		UUITools::setTextBlockText(this, "ReserveCount", "--");
		UUITools::setTextBlockColor(this, "ReserveCount", FLinearColor(0.25f, 0.25f, 0.25f, 1.0f));
	}

}

void UGnomeHUD::updateScoreChange(ADualAxisCreature* creature, float deltaTime) {
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

void UGnomeHUD::updateScoreBoard(float deltaTime) {

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	if (gameState->teams.Num() >= 1) {
		UTeam* team1 = gameState->teams[0];
		FLinearColor teamColor1 = FLinearColor(team1->color);
		teamColor1.A = 0.5f;
		UUITools::setBrushColor(this, "ScoreA_TeamColor1", FSlateColor(teamColor1));
		UUITools::setTextBlockText(this, "ScoreA_TeamScore1", FString::FromInt(team1->kills));

		UUITools::setVisibility(this, "ScoreA_Team1", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "ScoreA_Team1", ESlateVisibility::Collapsed);
	}

	if (gameState->teams.Num() >= 2) {
		UTeam* team2 = gameState->teams[1];
		FLinearColor teamColor2 = FLinearColor(team2->color);
		teamColor2.A = 0.5f;
		UUITools::setBrushColor(this, "ScoreA_TeamColor2", FSlateColor(teamColor2));
		UUITools::setTextBlockText(this, "ScoreA_TeamScore2", FString::FromInt(team2->kills));

		UUITools::setVisibility(this, "ScoreA_Team2", ESlateVisibility::Visible);
	}
	else {
		UUITools::setVisibility(this, "ScoreA_Team2", ESlateVisibility::Collapsed);
	}

}