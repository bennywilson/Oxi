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
	void EnterAttackState(TArray<AOxiCharacter*> EnemyList);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiSquadState SquadState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerceptionRadius;

	//UPROPERTY(EditAnywhere)
	//TArray<UOxiSquadAction> 
private:
	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentSquadMembers;
};

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OXI_API UOxiSquadAction : public UObject
{
	GENERATED_BODY()
};