// ELP 2022

#include "OxiAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "OxiHumanDamageComponent.h"


DEFINE_LOG_CATEGORY(LogOxiAI);

/**
 * 
 */
UOxiAIManager* GetOxiAIManager(AActor* ActorContext)
{
	UGameInstance* const GameInst = UGameplayStatics::GetGameInstance(ActorContext->GetWorld());
	if (GameInst == nullptr)
	{
		return nullptr;
	}

	return GameInst->GetSubsystem<UOxiAIManager>();
}

/**
 * 
 */
void AOxiSquad::AddSquadMember(AOxiCharacter* const SquadMemberToAdd)
{
	if (SquadMemberToAdd == nullptr)
	{
		UE_LOG(LogOxiAI, Warning, TEXT("UOxiSquad::AddSquadMember() - Tried to add null squad member"));
		return;
	}

	UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(SquadMemberToAdd->GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnKilledDelegate.AddUObject(this, &AOxiSquad::SquadMemberKilledCB);
	}
		
	CurrentSquadMembers.Add(SquadMemberToAdd);
}

/**
 * 
 */
void AOxiSquad::SquadMemberKilledCB(AActor* const Victim, AActor* const Killer)
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
void AOxiSquad::BeginPlay()
{
	Super::BeginPlay();

	GetOxiAIManager(this)->RegisterSquad(this);
 }

 /**
  * 
  */
void AOxiSquad::BeginDestroy()
{
	Super::BeginDestroy();

	UOxiAIManager* AIMgr = GetOxiAIManager(this);

	if (AIMgr != nullptr)
	{
		AIMgr->UnregisterSquad(this);
	}
}

/**
 *
 */
void AOxiSquad::ShutdownSquad()
{
	for (int i = 0; i < CurrentSquadMembers.Num(); i++)
	{
		GWorld->DestroyActor(CurrentSquadMembers[i]);
	}

	CurrentSquadMembers.Empty();

	GetOxiAIManager(this)->UnregisterSquad(this);
}

/**
 *
 */
int AOxiSquad::GetNumAliveSquadMembers() const
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
void UOxiAIManager::RegisterSquad(AOxiSquad* const Squad)
{
	SquadList.Add(Squad);
}

/**
 * 
 */
void UOxiAIManager::UnregisterSquad(AOxiSquad* const Squad)
{
	SquadList.Remove(Squad);
}