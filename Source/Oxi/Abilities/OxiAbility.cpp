// ELP 2023


#include "Abilities/OxiAbility.h"

/**
 *
 */
UOxiAbility::UOxiAbility()
{
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 *
 */
void UOxiAbility::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
 *
 */
void UOxiAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

