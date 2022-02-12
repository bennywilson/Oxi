// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiCover.h"

#include "OxiSquad.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiSquadState : uint8
{
	Idle,
	Patrol,
	Attack
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FOxiSquadTarget
{
	GENERATED_BODY()

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
	AOxiCharacter* Character;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
	FVector Location;
};

UCLASS(BlueprintType)
class OXI_API AOxiSquad : public AActor
{
	GENERATED_BODY()

public:
	AOxiSquad();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	void AddSquadMember(AOxiCharacter* const SquadMember);

	int GetNumSquadMembers() const { return CurrentSquadMembers.Num(); }
	int GetNumAliveSquadMembers() const;

private:
	void SquadMemberKilledCB(UOxiHumanDamageComponent* const DamageComp, AActor* const Victim, AActor* const Killer);
	void EnterAttackState();
	void TickAttackState(const float DeltaTime);

	void TickIdleState(const float DeltaTIme);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiSquadState SquadState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerceptionRadius;

	// Actions squad will take if the other lists are empty or their conditions haven't been met
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOxiSquadAction>> DefaultSquadActions;

	// Actions squad will take at the beginning of battle
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOxiSquadAction>> InitialSquadActions;

	// Actions squad will take when desperate (ex. losing the fight, low morale, etc)
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOxiSquadAction>> DesperateSquadActions;

	// Actions squad will take when winning (ex. player is hurt, high morale, etc)
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOxiSquadAction>> ConfidentSquadActions;

	// If a target leaves this radius, they are considered to have changed positions and the squad may call a new action
	UPROPERTY(EditAnywhere)
	float TargetsPositionRadius;

	UPROPERTY(BlueprintReadOnly, Transient)
	TArray<FOxiSquadTarget> SquadTargets;

	UPROPERTY(Transient, BlueprintReadonly)
	TArray<AOxiCharacter*> CurrentSquadMembers;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOxiSquadAction> DebugAction;
};

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OXI_API UOxiSquadAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeginAction(AOxiSquad* OxiSquad, UOxiAIManager*  AIManager);
};