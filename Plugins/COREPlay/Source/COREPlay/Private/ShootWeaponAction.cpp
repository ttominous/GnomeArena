#include "ShootWeaponAction.h"
#include "ActionInstance.h"
#include "ActionComponent.h"
#include "DualAxisCreature.h"
#include "Creature.h"
#include "CreatureAnimInstance.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "ProjectileWeaponItem.h"
#include "ProjectileWeaponItemSpec.h"
#include "Projectile.h"
#include "ProjectileBurst.h"
#include "MathTools.h"
#include "Debug.h"

bool UShootWeaponAction::canDoAction(AActor* actor) {

	UEquipmentComponent* equipment = UEquipmentComponent::get(actor);
	if (!equipment) {
		return false;
	}

	UInventoryComponent* inventory = UInventoryComponent::get(actor);
	if (!inventory) {
		return true;
	}

	UProjectileWeaponItem* item = Cast< UProjectileWeaponItem >(inventory->getEquippedItem("PrimaryWeapon"));
	if (!item) {
		return false;
	}

	return item->clipCount > 0;
}

void UShootWeaponAction::processStart(UActionInstance* instance, UActionComponent* component, FTargetDetail target) {

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UEquipmentComponent* equipment = UEquipmentComponent::get(component->GetOwner());
	if (!equipment) {
		return;
	}
	UProjectileWeaponItem* item = nullptr;
	UInventoryComponent* inventory = UInventoryComponent::get(component->GetOwner());
	if (inventory) {
		item = Cast< UProjectileWeaponItem >(inventory->getEquippedItem("PrimaryWeapon"));
	}
	UProjectileWeaponItemSpec* itemSpec = item != nullptr ? Cast< UProjectileWeaponItemSpec >(item->getItemSpec()) : Cast< UProjectileWeaponItemSpec >(equipment->getEquippedItemSpec("PrimaryWeapon"));
	if (!itemSpec) {
		return;
	}

	UPlayAnimActionInstance* playAnimInstance = Cast< UPlayAnimActionInstance >(instance);
	UCreatureAnimInstance* animInstance = creature->getAnimInstance();
	if (creature->isAiming() && itemSpec->animationDatas.Contains("ShootAiming")) {
		playAnimInstance->animSequence = playAnimation(itemSpec->animationDatas["ShootAiming"], creature);
		if (animInstance) {
			animInstance->processPlayAnimStart(itemSpec->animationDatas["ShootAiming"]);
		}
	}
	else if(itemSpec->animationDatas.Contains("Shoot")) {
		playAnimInstance->animSequence = playAnimation(itemSpec->animationDatas["Shoot"], creature);
		if (animInstance) {
			animInstance->processPlayAnimStart(itemSpec->animationDatas["Shoot"]);
		}
	}

	FVector weaponTip = creature->getWeaponTip();

	if (!creature->GetWorld()->IsNetMode(ENetMode::NM_Client)) {

		if (item) {
			item->clipCount -= 1;
		}

		FRotator direction = UMathTools::getYawAndPitch(weaponTip, creature->aimAtPoint);
		float aimAccuracy = creature->getAimAccuracy() * 3.5f;

		for (FProjectileBurst& burst : itemSpec->projectiles) {
			int count = FMath::RandRange(burst.minCount, burst.maxCount);
			if (count <= 0) {
				continue;
			}

			for (int i = 0; i < count; i++) {
				FVector location = weaponTip + UMathTools::randomVectorFromRange(burst.minLocationOffset, burst.maxLocationOffset);
				FRotator rotation = direction + UMathTools::randomRotatorFromRange(burst.minRotationOffset, burst.maxRotationOffset) + UMathTools::randomRotatorFromRange(aimAccuracy, aimAccuracy, aimAccuracy);



				AProjectile::spawn(creature, creature->GetWorld(), burst.projectileSpec, location, rotation, burst.speedModifier);
			}
		}
	}

	if (itemSpec->effects.Contains("Shoot") && itemSpec->effects["Shoot"] != nullptr) {
		itemSpec->effects["Shoot"]->execute(creature, FVector::ZeroVector, FRotator(180, 90, 0), FVector(0.1f), "Tip", creature->getWeaponComponent());
	}
}

void UShootWeaponAction::processAnimationFinished(UActionInstance* instance, UActionComponent* component) {
	Super::processAnimationFinished(instance, component);

	ADualAxisCreature* creature = Cast< ADualAxisCreature >(component->GetOwner());
	if (!creature) {
		return;
	}

	UEquipmentComponent* equipment = UEquipmentComponent::get(creature);
	if (!equipment) {
		return;
	}
	UProjectileWeaponItemSpec* itemSpec = Cast< UProjectileWeaponItemSpec >(equipment->getEquippedItemSpec("PrimaryWeapon"));
	if (!itemSpec) {
		return;
	}

	if (itemSpec->pumpAfterShooting) {
		component->startBaseAction("PumpWeapon");
	}
}