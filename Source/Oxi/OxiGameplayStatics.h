// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OxiGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class OXI_API UOxiGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static void PrintMessageToConsole(const FString& Msg);

	UFUNCTION(BlueprintCallable)
	static FRotator WeightedMovingAverage(const FRotator& Cur, const FRotator& prev, const float weight);

};
