// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiCover.h"

#include "OxiAIManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOxiAI, Warning, All);

/**
 *
 */
UENUM(BlueprintType)
enum class OxiAICommand : uint8
{
	None,
	TakeCover,
	RushTarget,
	FlankTarget,
	HoldPosition,
};

USTRUCT(BlueprintType)
struct FOxiAICommandData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	OxiAICommand AICommand;

	UPROPERTY(BlueprintReadWrite)
	AActor* Target;

	UPROPERTY(BlueprintReadWrite)
	AActor* Goal;
};

/**
 *
 */
UCLASS(BlueprintType)
class OXI_API AOxiAICharacter : public AOxiCharacter
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IssueAICommand(const FOxiAICommandData& CommandData);

	UFUNCTION(BlueprintCallable)
	bool HasReachedDestination();

	UFUNCTION(BlueprintCallable)
	class AOxiCover* FindAndAcquireCover();

protected:

	virtual void OnCoverProtectionLevelChanged(AOxiCover* const, EOxiCoverProtectionLevel) override;

protected:
	
	UPROPERTY(Transient, BlueprintReadWrite)
	FOxiAICommandData CurrentAICommand;
};

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

private:

	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentSquadMembers;
};

/**
 *
 */
UCLASS()
class OXI_API UOxiAIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void RegisterSquad(AOxiSquad* const Squad);
	void UnregisterSquad(AOxiSquad* const Squad);

	void RegisterCover(AOxiCover* const Cover);
	void UnregisterCover(AOxiCover* const Cover);

	void RegisterPlayer(AOxiFirstPersonCharacter* const Player);
	void UnregisterPlayer(AOxiFirstPersonCharacter* const Player);

	const TArray<AOxiFirstPersonCharacter*> GetPlayerList() const { return PlayerList; }

	TArray<AOxiCover*> GetCoverList() const { return CoverList; }

private:

	UPROPERTY(Transient)
	TArray<AOxiFirstPersonCharacter*> PlayerList;

	UPROPERTY(Transient)
	TArray<AOxiSquad*> SquadList;

	UPROPERTY(Transient)
	TArray<AOxiCover*> CoverList;
};

extern UOxiAIManager * GetOxiAIManager(AActor* ActorContext);