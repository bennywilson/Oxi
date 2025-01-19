// ELP 2023


#include "OxiAbility.h"

static TAutoConsoleVariable<int32> CVarAbilityDebug(
	TEXT("oxi.abilityDebug"),
	0,
	TEXT("Show debug squad indo"),
	ECVF_Cheat
);

/**
 *
 */
UOxiAbility::UOxiAbility()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

/**
 *
 */
void UOxiAbility::BeginPlay()
{
	Super::BeginPlay();
	check(MinCoolDownSec >= 0 && MaxCoolDownSec >= MinCoolDownSec);

	AbilityState = EOxiAbilityState::Ready;
	CurrentNumOfCharges = MaxNumCharges;
	SetActive(false);
}

/**
 *
 */
void UOxiAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
 *
 */
EOxiAbilityFailReason UOxiAbility::StartAbility()
{
	if (AbilityState == EOxiAbilityState::CoolDown || AbilityState == EOxiAbilityState::ChargeCoolDown)
	{
		return EOxiAbilityFailReason::CoolDown;
	}

	if (AbilityState == EOxiAbilityState::Running)
	{
		return EOxiAbilityFailReason::AlreadyRunning;
	}

	AbilityStartTimeSec = GetWorld()->GetTimeSeconds();
	AbilityState = EOxiAbilityState::Running;

	DurationTimerDelegate.BindUFunction(this, FName("StopAbility"), EOxiAbilityStopReason::Finished);

	if (DurationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	AbilityDurationSec = FMath::RandRange(MinDurationSec, MaxDurationSec);
	GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, DurationTimerDelegate, AbilityDurationSec, false);

	if (MaxNumCharges > 0)
	{
		CurrentNumOfCharges--;
	}

	StartAbility_Internal();

	return EOxiAbilityFailReason::Succeeded;
}

/**
 *
 */
void UOxiAbility::StopAbility(const EOxiAbilityStopReason stopReason)
{
	StopAbility_Internal(stopReason);

	if (CurrentNumOfCharges > 0)
	{ 
		if (SecondsBetweenCharges > 0)
		{
			AbilityState = EOxiAbilityState::ChargeCoolDown;
			ChargeCoolDownTimerDelegate.BindUFunction(this, FName("ChargeCoolDownFinishedCB"));

			if (ChargeCoolDownTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(ChargeCoolDownTimerHandle);
			}
			GetWorld()->GetTimerManager().SetTimer(ChargeCoolDownTimerHandle, ChargeCoolDownTimerDelegate, SecondsBetweenCharges, false);
		}
		else
		{
			AbilityState = EOxiAbilityState::Ready;
		}
	}
	else
	{
		const float coolDownTime = FMath::RandRange(MinCoolDownSec, MaxCoolDownSec);
		if (coolDownTime > 0)
		{
			AbilityState = EOxiAbilityState::CoolDown;
			CoolDownTimerDelegate.BindUFunction(this, FName("CooldownFinishedCB"));

			if (CoolDownTimerHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(CoolDownTimerHandle);
			}

			CoolDownStartTime = GetWorld()->GetTimeSeconds();
			GetWorld()->GetTimerManager().SetTimer(CoolDownTimerHandle, CoolDownTimerDelegate, coolDownTime, false);

		}
		else
		{
			AbilityState = EOxiAbilityState::Ready;
		}
	}
	AbilityStartTimeSec = 0;
}

/**
 *
 */
void UOxiAbility::CoolDownFinishedCB()
{
	AbilityState = EOxiAbilityState::Ready;
	CoolDownFinished_Internal();
}

/**
 *
 */
void UOxiAbility::ChargeCoolDownFinishedCB()
{
	AbilityState = EOxiAbilityState::Ready;
}

/**
 *
 */
float UOxiAbility::GetNormalizedRunningTime()
{
	if (AbilityState == EOxiAbilityState::Ready)
	{
		return 0.0f;
	}

	if (AbilityState == EOxiAbilityState::CoolDown)
	{
		return 1.0f;
	}

	if (AbilityDurationSec <= 0.0f)
	{
		return 1.0f;
	}

	const float curTime = GetWorld()->GetTimeSeconds();
	return (curTime - AbilityStartTimeSec) / AbilityDurationSec;
}

/**
 *
 */
UOxiAbilitySystem::UOxiAbilitySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 *
 */
void UOxiAbilitySystem::BeginPlay()
{
	Super::BeginPlay();

}

/**
 *
 */
void UOxiAbilitySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
 *
 */
const UOxiAbility* UOxiAbilitySystem::GetAbilityByType(const EOxiAbilityType abilityType)
{
	for (int i = 0; i < AbilitiesList.Num(); i++)
	{ 
		const UOxiAbility* const curAbility = AbilitiesList[i];
		if (curAbility->GetAbilityType() == abilityType)
		{
			return curAbility;
		}
	}

	return nullptr;
}

/**
 *
 */
TArray<UOxiAbility*> UOxiAbilitySystem::GetAbilitiesByType(const EOxiAbilityType abilityType)
{
	TArray<UOxiAbility*> returnList;

	for (int i = 0; i < AbilitiesList.Num(); i++)
	{
		UOxiAbility* const curAbility = AbilitiesList[i];
		if (curAbility->GetAbilityType() == abilityType)
		{
			returnList.Add(curAbility);
		}
	}

	return returnList;
}