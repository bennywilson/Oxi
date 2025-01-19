// ELP 2020

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OxiHUD.generated.h"

UCLASS()
class AOxiHUD : public AHUD
{
	GENERATED_BODY()

public:

	AOxiHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:

};

