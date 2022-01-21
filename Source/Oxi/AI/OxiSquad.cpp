// ELP 2022

#include "OxiSquad.h"
#include "Kismet/GameplayStatics.h"
#include "OxiAIManager.h"
#include "OxiHumanDamageComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

/**
  * 
  */
	AOxiSquad::AOxiSquad()
{
	PrimaryActorTick.bCanEverTick = true;
}

/**
 *
 */
void AOxiSquad::AddSquadMember(AOxiCharacter* const SquadMemberToAdd)
{
	check(SquadMemberToAdd);

	UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(SquadMemberToAdd->GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnDeath.AddUObject(this, &AOxiSquad::SquadMemberKilledCB);
	}

	CurrentSquadMembers.Add(SquadMemberToAdd);
}

/**
 *
 */
void AOxiSquad::SquadMemberKilledCB(UOxiHumanDamageComponent* const DamageComp, AActor* const Victim, AActor* const Killer)
{
	check(DamageComp);

	AOxiCharacter* const OxiChar = Cast<AOxiCharacter>(Victim);
	check(OxiChar != nullptr);

	DamageComp->OnDeath.RemoveAll(this);
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
void AOxiSquad::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (int i = 0; i < CurrentSquadMembers.Num(); i++)
	{
		GWorld->DestroyActor(CurrentSquadMembers[i]);
	}

	CurrentSquadMembers.Empty();

	UOxiAIManager* AIMgr = GetOxiAIManager(this);
	AIMgr->UnregisterSquad(this);
}

/**
 *
 */
void AOxiSquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	if (SquadState != EOxiSquadState::Attack)
	{
		TArray<AOxiFirstPersonCharacter*> PlayerList = AIMgr->GetPlayerList();
		if (PlayerList.Num() > 0)
		{
			AOxiFirstPersonCharacter* const Player = PlayerList[0];
			for (int i = 0; i < CurrentSquadMembers.Num(); i++)
			{
				AOxiCharacter* const SquadMember = CurrentSquadMembers[i];
				if (FVector::Dist(SquadMember->GetActorLocation(), Player->GetActorLocation()) <= PerceptionRadius)
				{
					TArray<AOxiCharacter*> EnemyList;
					EnemyList.Add(Player);
					EnterAttackState(EnemyList);
					break;
				}
			}
		}
	}
}

/**
 *
 */
void AOxiSquad::EnterAttackState(TArray<AOxiCharacter *> EnemyList)
{
	check(EnemyList.Num() > 0);

	SquadState = EOxiSquadState::Attack;

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	TArray<AOxiCover*> CoverList = AIMgr->GetCoverList();
	if (CoverList.Num() == 0)
	{
		FOxiAICommandData AICommandData;
		AICommandData.AICommand = OxiAICommand::HoldPosition;
		AICommandData.Target = EnemyList[0];

		for (int i = 0; i < CurrentSquadMembers.Num(); i++)
		{
			AOxiAICharacter* const SquadMember = Cast<AOxiAICharacter>(CurrentSquadMembers[i]);
			SquadMember->IssueAICommand(AICommandData);
		}
	}

	for (int iSquad = 0; iSquad < CurrentSquadMembers.Num(); iSquad++)
	{
		AOxiAICharacter* const SquadMember = Cast<AOxiAICharacter>(CurrentSquadMembers[iSquad]);

		AOxiCover* const NearestCover = FindNearestUnusedCover(CoverList, SquadMember->GetActorLocation());
		if (NearestCover == nullptr)
		{
			// Hold current position
			FOxiAICommandData AICommandData;
			AICommandData.AICommand = OxiAICommand::HoldPosition;
			AICommandData.Target = EnemyList[0];
			AICommandData.Goal = nullptr;
			SquadMember->IssueAICommand(AICommandData);
		}
		else
		{
			// Take Cover
			FOxiAICommandData AICommandData;
			AICommandData.AICommand = OxiAICommand::TakeCover;
			AICommandData.Target = EnemyList[0];
			AICommandData.Goal = NearestCover;

			CoverList.Remove(NearestCover);

			SquadMember->AcquireCover(NearestCover);	// TODO - put inside of blueprint
			SquadMember->IssueAICommand(AICommandData);
		}
	}
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