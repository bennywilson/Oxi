// ELP 2020

#pragma once

#include "CoreMinimal.h"
#include "OxiDamageComponent.h"
#include "OxiHumanDamageComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, editinlinenew, meta = (BlueprintSpawnableComponent))
class OXI_API UOxiHumanDamageComponent : public UOxiDamageComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:

	virtual float TakeDamage_Internal(const FOxiDamageInfo& DamageInfo) override;

	bool bRagdolling = false;
};

