// ELP 2022

#include "OxiAIManager.h"
#include "OxiHumanDamageComponent.h"


DEFINE_LOG_CATEGORY(LogOxiAI);

/**
 * 
 */
void UOxiSquad::AddSquadMember(AOxiCharacter* const SquadMemberToAdd)
{
	if (SquadMemberToAdd == nullptr)
	{
		UE_LOG(LogOxiAI, Warning, TEXT("UOxiSquad::AddSquadMember() - Tried to add null squad member"));
		return;
	}

	UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(SquadMemberToAdd->GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnKilledDelegate.AddUObject(this, &UOxiSquad::SquadMemberKilledCB);
	}
		
	CurrentSquadMembers.Add(SquadMemberToAdd);
}

/**
 * 
 */
void UOxiSquad::SquadMemberKilledCB(AActor* const Victim, AActor* const Killer)
{
	AOxiCharacter* const OxiChar = Cast<AOxiCharacter>(Victim);
	check(OxiChar != nullptr);

	UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(OxiChar->GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnKilledDelegate.RemoveAll(this);
	}
}

/**
 *
 */
void UOxiSquad::BeginDestroy()
{
	Super::BeginDestroy();
	for (int i = 0; i < CurrentSquadMembers.Num(); i++)
	{
		GWorld->DestroyActor(CurrentSquadMembers[i]);
	}

	CurrentSquadMembers.Empty();
}

/**
 *
 */
int UOxiSquad::GetNumAliveSquadMembers() const
{
	int NumAlive = 0;

	for (int i = 0; i < CurrentSquadMembers.Num(); i++)
	{
		AOxiCharacter* const SquadMember = CurrentSquadMembers[i];
		UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(SquadMember->GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
		if (DamageComp == nullptr || DamageComp->IsAlive())
		{
			NumAlive++;
		}
	}

	return NumAlive;
}

/**
 * 
 */
void UOxiAISquadManager::RegisterSquad(UOxiSquad* const Squad)
{

}

/**
 * 
 */
void UOxiAISquadManager::UnregisterSquad(UOxiSquad* const Squad)
{

}