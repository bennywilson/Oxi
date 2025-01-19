// Fill out your copyright notice in the Description page of Project Settings.


#include "OxiGameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>

/**
 *
 */
void UOxiGameplayStatics::PrintMessageToConsole(const FString& Msg)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
};

/**
 *
 */
float NormalizeAngle(const float angle)
{
	return fmod(angle, 360.f);
}

float AngleDelta(float Cur, float Prev)
{
	Cur = NormalizeAngle(Cur);
	Prev = NormalizeAngle(Prev);
	const float delta = Cur - Prev;
	if (delta >= 180.f) {
		return delta - 360.f;
	} else if (delta < -180.f) {
		return delta + 360.f;
	}

	return delta;

}

FRotator UOxiGameplayStatics::WeightedMovingAverage(const FRotator& Cur, const FRotator& Prev, const float Weight)
{
	FRotator OutRotator;
	OutRotator.Yaw = NormalizeAngle(Prev.Yaw + AngleDelta(Cur.Yaw, Prev.Yaw) * Weight);
	OutRotator.Pitch = NormalizeAngle(Prev.Pitch + AngleDelta(Cur.Pitch, Prev.Pitch) * Weight);
	OutRotator.Roll = NormalizeAngle(Prev.Roll + AngleDelta(Cur.Roll, Prev.Roll) * Weight);
	return OutRotator;
}
