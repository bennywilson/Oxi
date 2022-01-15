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
class OXI_API UOxiSquad : public UObject
{
	GENERATED_BODY()

public:

	virtual void BeginDestroy() override;

	void AddSquadMember(AOxiCharacter* const SquadMember);

	int GetNumSquadMembers() const { return CurrentSquadMembers.Num(); }
	int GetNumAliveSquadMembers() const;

private:

	void SquadMemberKilledCB(AActor* const Victim, AActor* const Killer);

	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentSquadMembers;
};

UCLASS()
class OXI_API UOxiAISquadManager : public UObject
{
	GENERATED_BODY()
	
	void RegisterSquad(UOxiSquad* const Squad);
	void UnregisterSquad(UOxiSquad* const Squad);

private:

	UPROPERTY(Transient)
	TArray<UOxiSquad*> SquadList;
};
