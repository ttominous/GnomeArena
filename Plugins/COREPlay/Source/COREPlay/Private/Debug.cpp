#include "Debug.h"
#include "DebugTextWidget.h"
#include "TimeTools.h"

void UDebug::log(FString text) {
	text = "[" + UTimeTools::getSytemDateTimeString() + "] " + text;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *text);
}

void UDebug::print(FString text) {
	if (UDebugTextWidget::instance) {
		UDebugTextWidget::addText(text, FColor::White, 5.0f);
		return;
	}
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, *text);
	}
}

void UDebug::print(FString text, FColor color) {
	if (UDebugTextWidget::instance) {
		UDebugTextWidget::addText(text, color, 5.0f);
		return;
	}
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, color, *text);
	}
}

void UDebug::print(FString text, FColor color, float time) {
	if (UDebugTextWidget::instance) {
		UDebugTextWidget::addText(text, color, time);
		return;
	}
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, time, color, *text);
	}
}

void UDebug::error(FString text) {
	print("[ERROR] " + text, FColor::Red);
}

void UDebug::netError(UObject* object, FString text) {
	netError(object ? object->GetWorld() : nullptr, text);
}

void UDebug::netError(UWorld* world, FString text) {
	netPrint(world, "[ERROR] " + text, FColor::Red);
}

void UDebug::netPrint(UObject* object, FString text, FColor color, float time) {
	netPrint(object ? object->GetWorld() : nullptr, text, color, time);
}

void UDebug::netPrint(UWorld* world, FString text, FColor color, float time) {

	if (!world) {
		print("[NO-WORLD] " + text, color, time);
		return;
	}

	if (!world->IsNetMode(ENetMode::NM_Client)) {
		print("[SERVER] " + text, color, time);
	}
	else {
		print("[CLIENT] " + text, color, time);
	}
}

void UDebug::tick(FString text) {
	print(text, FColor::Yellow, 0.0075f);
}

void UDebug::tick(FString text, FColor color) {
	print(text, color, 0.0075f);
}

void UDebug::netTick(UWorld* world, FString text) {
	netPrint(world, text, FColor::Yellow, 0.0075f);
}

void UDebug::drawSphere(UWorld* world, FVector location, float radius, FColor color, float lifeSpan) {
	DrawDebugSphere(world, location, radius, 32, color, false, lifeSpan, 0, 1.0f);
}

void UDebug::drawBox(UWorld* world, FVector min, FVector max, FColor color, float lineThickness, float lifeSpan) {
	drawBox(world, FBox(min, max), color, lineThickness, lifeSpan);
}

void UDebug::drawBox(UWorld* world, FBox box, FColor color, float lineThickness, float lifeSpan) {
	DrawDebugBox(world, box.GetCenter(), box.GetExtent(), color, false, lifeSpan, 0, lineThickness);
}

void UDebug::drawBox(UWorld* world, FVector location, FQuat rotation, FVector size, FColor color, float lineThickness, float lifeSpan) {
	DrawDebugBox(world, location, size, rotation, color, false, lifeSpan, 0, lineThickness);
}

void UDebug::drawLine(UWorld* world, FVector start, FVector end, FColor color, float thickness, float lifeSpan) {
	DrawDebugLine(world, start, end, color, false, lifeSpan, 0, thickness);
}

void UDebug::drawPoint(UWorld* world, FVector location, float radius, FColor color, float lifeSpan) {
	DrawDebugPoint(world, location, radius, color, false, lifeSpan, 0);
}

void UDebug::drawText(UWorld* world, FVector location, FString text, FColor color, float lifeSpan) {
	DrawDebugString(world, location, *text, NULL, color, lifeSpan, false);
}