// ELP 2023


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OxiAbility.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiAbilityState : uint8
{
	Ready,
	CoolDown,
	Running
};

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiAbilityFailReason : uint8
{
	Succeeded,
	CoolDown,
	AlreadyRunning
};

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiAbilityStopReason : uint8
{
	Finished,
	Cancelled,
	Interuppted,
};

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiAbilityType : uint8
{
	Dash
};

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OXI_API UOxiAbility : public UActorComponent
{
	GENERATED_BODY()

public:
	UOxiAbility();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	EOxiAbilityFailReason StartAbility();

	UFUNCTION(BlueprintCallable)
	void StopAbility(const EOxiAbilityStopReason stopReason);

	EOxiAbilityType GetAbilityType() const { return AbilityType; }

	EOxiAbilityState GetAbilityState() const { return AbilityState; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartAbility_Internal();

	UFUNCTION(BlueprintImplementableEvent)
	void StopAbility_Internal(const EOxiAbilityStopReason stopReason);

	UFUNCTION()
	void CoolDownFinishedCB();

	UFUNCTION(BlueprintImplementableEvent)
	void CoolDownFinished_Internal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOxiAbilityType AbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinCoolDownSec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCoolDownSec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDurationSec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDurationSec;

	UPROPERTY(Transient)
	float AbilityStartTime;

	UPROPERTY(Transient)
	EOxiAbilityState AbilityState;

private:
	FTimerDelegate DurationTimerDelegate;
	FTimerHandle DurationTimerHandle;

	FTimerDelegate CoolDownTimerDelegate;
	FTimerHandle CoolDownTimerHandle;
};

/**
 *
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OXI_API UOxiAbilitySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOxiAbilitySystem();

	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	const UOxiAbility* GetAbilityByType(const EOxiAbilityType abilityType);

	UFUNCTION(BlueprintCallable)
	TArray<UOxiAbility*> GetAbilitiesByType(const EOxiAbilityType abilityType);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditInline="true"))
	TArray<UOxiAbility*> AbilitiesList;
};
