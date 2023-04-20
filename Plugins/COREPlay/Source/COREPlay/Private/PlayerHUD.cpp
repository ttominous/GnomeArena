#include "PlayerHUD.h"
#include "ComplexGameState.h"
#include "Creature.h"
#include "Team.h"
#include "PlayerRecord.h"
#include "ResourceCache.h"
#include "UITools.h"
#include "MathTools.h"
#include "Debug.h"

#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"


void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	AComplexGameState* gameState = Cast< AComplexGameState >(GetWorld()->GetGameState());
	if (!gameState) {
		return;
	}

	APlayerController* controller = GetOwningPlayer();
	bool scoreboardActive = controller->IsInputKeyDown(FKey("Tab")) || controller->IsInputKeyDown(FKey("Gamepad_Special_Left"));
	UUITools::setVisibility(this, "Scoreboard", scoreboardActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	updateDeathAlerts(InDeltaTime);
}


void UPlayerHUD::updateDeathAlerts(float deltaTime) {

	for (int i = 0; i < allDeathAlerts.Num(); i++) {
		allDeathAlerts[i].timeLeft -= deltaTime;
		if (allDeathAlerts[i].timeLeft <= 0.0f || maxAllDeathAlertsToDisplay < allDeathAlerts.Num()) {
			allDeathAlerts.RemoveAt(i);
			i--;
		}
	}

	if (deathAlertWidgetClass == nullptr) {
		return;
	}

	UVerticalBox* deathAlertsContainer = UUITools::getVerticalBox(this, "DeathAlertsContainer");
	if (!deathAlertsContainer) {
		return;
	}

	UUITools::syncVerticalBoxChildren(deathAlertsContainer, allDeathAlerts.Num(), deathAlertWidgetClass);

	TArray< UWidget* > 	alertEntries = deathAlertsContainer->GetAllChildren();
	for (int i = 0; i < alertEntries.Num(); i++) {
		UUserWidget* alertWidget = Cast< UUserWidget >(alertEntries[i]);
		if (alertWidget && allDeathAlerts.Num() > i) {
			float opacity = FMath::Min(1.0f,(allDeathAlerts[i].timeLeft / allDeathAlertTime) + 0.75f);
			alertWidget->SetRenderOpacity(opacity);

			if (allDeathAlerts[i].instigatorName.Len() == 0) {
				UUITools::setVisibility(alertWidget, "AttackerName", ESlateVisibility::Collapsed);
			}
			else {
				UUITools::setTextBlockText(alertWidget, "AttackerName", allDeathAlerts[i].instigatorName);
				UUITools::setTextBlockColor(alertWidget, "AttackerName", allDeathAlerts[i].instigatorColor);
				UUITools::setVisibility(alertWidget, "AttackerName", ESlateVisibility::Visible);
			}


			if (allDeathAlerts[i].targetName.Len() == 0) {
				UUITools::setVisibility(alertWidget, "VictimName", ESlateVisibility::Collapsed);
			}
			else {
				UUITools::setTextBlockText(alertWidget, "VictimName", allDeathAlerts[i].targetName);
				UUITools::setTextBlockColor(alertWidget, "VictimName", allDeathAlerts[i].targetColor);
				UUITools::setVisibility(alertWidget, "VictimName", ESlateVisibility::Visible);
			}


			if (allDeathAlerts[i].message.Len() == 0) {
				UUITools::setVisibility(alertWidget, "Description", ESlateVisibility::Collapsed);
			}
			else {
				UUITools::setTextBlockText(alertWidget, "Description", allDeathAlerts[i].message);
				UUITools::setVisibility(alertWidget, "Description", ESlateVisibility::Visible);
			}


			if (allDeathAlerts[i].icon == nullptr) {
				UUITools::setVisibility(alertWidget, "Icon", ESlateVisibility::Collapsed);
			}
			else {
				UUITools::setImageBrushTexture(alertWidget, "Icon", allDeathAlerts[i].icon);
				UUITools::setVisibility(alertWidget, "Icon", ESlateVisibility::Visible);
			}
		}
	}

}


void UPlayerHUD::populateAlertVictim(FPlayerHUDScoreAlert& alert, FTargetDetail& target) {
	if (target.type == ETargetClass::CREATURE) {
		ACreature* creature = target.getCreature(GetWorld());
		if (creature == nullptr) {
			return;
		}
		UPlayerRecord* player = UPlayerRecord::get(creature);
		UTeam* team = player != nullptr ? UTeam::get(player) : UTeam::get(creature);

		alert.targetName = player != nullptr ? player->displayName : creature->name;
		alert.targetColor = team != nullptr ? team->color : defaultNoTeamColor;
	}
}

void UPlayerHUD::populateAlertAttacker(FPlayerHUDScoreAlert& alert, FTargetDetail& target) {
	if (target.type == ETargetClass::CREATURE) {
		ACreature* creature = target.getCreature(GetWorld());
		if (creature == nullptr) {
			return;
		}
		UPlayerRecord* player = UPlayerRecord::get(creature);
		UTeam* team = player != nullptr ? UTeam::get(player) : UTeam::get(creature);

		alert.instigatorName = player != nullptr ? player->displayName : creature->name;
		alert.instigatorColor = team != nullptr ? team->color : defaultNoTeamColor;
	}
}

void UPlayerHUD::populateKillAlertDescription(FPlayerHUDScoreAlert& alert, FDamageDetails& damage) {
	alert.message = damage.description;

	if (damage.iconKey.Len() > 0) {
		alert.icon = UResourceCache::getTexture(damage.iconKey);
	}
	if (alert.icon == nullptr) {
		alert.icon = defaultKillIcon;
	}
}


void UPlayerHUD::alertKill(FDamageDetails damage) {
	FPlayerHUDScoreAlert alert;

	ACreature* attacker = damage.attacker.getCreature(GetWorld());
	if (attacker == GetOwningPlayer()->GetPawn()) {
		UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("ScorePoint"), 1.0f, 1.0f, 0.0f);
	}

	ACreature* victim = damage.victim.getCreature(GetWorld());
	if (victim == GetOwningPlayer()->GetPawn()) {
		UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("ScoreDeath"), 1.0f, 1.0f, 0.0f);
	}
	
	populateAlertVictim(alert, damage.victim);
	if (alert.targetName.Len() == 0) {
		return;
	}
	populateAlertAttacker(alert, damage.attacker);
	populateKillAlertDescription(alert, damage);

	alert.timeLeft = allDeathAlertTime;
	allDeathAlerts.Add(alert);
}

void UPlayerHUD::alertToTarget(FTargetDetail target, FString message, FColor color, FString iconKey) {
	FPlayerHUDScoreAlert alert;

	ACreature* victim = target.getCreature(GetWorld());
	if (victim == GetOwningPlayer()->GetPawn()) {
		UGameplayStatics::PlaySound2D(GetWorld(), UResourceCache::getSound("ScoreDeath"), 1.0f, 1.0f, 0.0f);
	}

	populateAlertVictim(alert, target);
	if (alert.targetName.Len() == 0) {
		return;
	}

	alert.instigatorName = message;
	alert.instigatorColor = color;
	alert.icon = UResourceCache::getTexture(iconKey);

	alert.timeLeft = allDeathAlertTime;
	allDeathAlerts.Add(alert);
}

void UPlayerHUD::alert(FString message, FColor color, FString iconKey) {
	FPlayerHUDScoreAlert alert;

	alert.instigatorName = message;
	alert.instigatorColor = color;
	alert.icon = UResourceCache::getTexture(iconKey);

	alert.timeLeft = allDeathAlertTime;
	allDeathAlerts.Add(alert);
}