// ELP 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "OxiCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class OXI_API UOxiCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(exec, BlueprintCallable, Category = "Cheat Manager")
	virtual void WarpToCheckPoint(FString CheckPointName);
};
