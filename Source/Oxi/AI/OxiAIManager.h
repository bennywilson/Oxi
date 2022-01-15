// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiAIManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOxiAI, Warning, All);

UCLASS()
class OXI_API AOxiAICharacter : public AOxiCharacter
{
	GENERATED_BODY()
};

UCLASS()
class OXI_API AOxiSquad : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	void AddSquadMember(AOxiCharacter* const SquadMember);
	void ShutdownSquad();

	int GetNumSquadMembers() const { return CurrentSquadMembers.Num(); }
	int GetNumAliveSquadMembers() const;

private:

	void SquadMemberKilledCB(AActor* const Victim, AActor* const Killer);

	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentSquadMembers;
};

UCLASS()
class OXI_API UOxiAIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void RegisterSquad(AOxiSquad* const Squad);
	void UnregisterSquad(AOxiSquad* const Squad);

private:

	UPROPERTY(Transient)
	TArray<AOxiSquad*> SquadList;
};

extern UOxiAIManager * GetOxiAIManager(AActor* ActorContext);