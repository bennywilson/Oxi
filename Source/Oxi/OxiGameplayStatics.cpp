// Fill out your copyright notice in the Description page of Project Settings.


#include "OxiGameplayStatics.h"

void UOxiGameplayStatics::PrintMessageToConsole(const FString& Msg)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
};