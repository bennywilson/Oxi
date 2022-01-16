// ELP 2022

#include "OxiAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "OxiHumanDamageComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogOxiAI);

/**
 *	Helper functions
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

AOxiCover* FindNearestUnusedCover(TArray<AOxiCover*> CoverList, const FVector TestPoint)
{
	float ClosestCoverDist = FLT_MAX;
	int ClosestCoverIdx = -1;
	for (int iCover = 0; iCover < CoverList.Num(); iCover++)
	{
		AOxiCover* const CurrentCover = CoverList[iCover];
		if (CurrentCover->GetNumUsers() > 0)
		{
			continue;
		}

		if (CurrentCover->GetCoverProtectionLevel() == EOxiCoverProtectionLevel::Broken)
		{
			continue;
		}

		float CoverDist = FVector::Dist(CurrentCover->GetActorLocation(), TestPoint);
		if (CoverDist < ClosestCoverDist)
		{
			ClosestCoverIdx = iCover;
			ClosestCoverDist = CoverDist;
		}
	}

	if (ClosestCoverIdx == -1)
	{
		return nullptr;
	}

	return CoverList[ClosestCoverIdx];
}

/**
 *
 */
bool AOxiAICharacter::HasReachedDestination()
{
	AAIController* const AIController = Cast<AAIController>(GetController());
	UPathFollowingComponent* const PathComponent = AIController->GetPathFollowingComponent();
	return PathComponent->DidMoveReachGoal();
}


/**
 * 
 */
AOxiCover* AOxiAICharacter::FindAndAcquireCover()
{
	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	TArray<AOxiCover*> CoverList = AIMgr->GetCoverList();
	AOxiCover* const NewCover = FindNearestUnusedCover(CoverList, GetActorLocation());
	if (NewCover == nullptr)
	{
		return nullptr;
	}

	AcquireCover(NewCover);

	return NewCover;
}

/**
 *
 */
void AOxiAICharacter::OnCoverProtectionLevelChanged(AOxiCover* const Cover, EOxiCoverProtectionLevel NewProtectionLevel)
{
	check(Cover);
	if (NewProtectionLevel == EOxiCoverProtectionLevel::Broken)
	{
		// Cover is broken. Find new cover
		ReleaseCover();
		AOxiCover* const NearestCover = FindAndAcquireCover();
		if (NearestCover != nullptr)
		{
			// Take Cover
			FOxiAICommandData AICommandData;
			AICommandData.AICommand = OxiAICommand::TakeCover;
			AICommandData.Target = CurrentAICommand.Target;
			AICommandData.Goal = NearestCover;

			IssueAICommand(AICommandData);
		}
	}
}

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
	if (SquadMemberToAdd == nullptr)
	{
		UE_LOG(LogOxiAI, Warning, TEXT("UOxiSquad::AddSquadMember() - Tried to add null squad member"));
		return;
	}

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
	check(Victim);
	AOxiCharacter* const OxiChar = Cast<AOxiCharacter>(Victim);
	check(OxiChar != nullptr);

	check(DamageComp);
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
	if (AIMgr != nullptr)
	{
		AIMgr->UnregisterSquad(this);
	}
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

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	SquadState = EOxiSquadState::Attack;

	TArray<AOxiCover*> CoverList = AIMgr->GetCoverList();
	if (CoverList.Num() == 0)
	{
		FOxiAICommandData AICommandData;
		AICommandData.AICommand = OxiAICommand::HoldPosition;
		AICommandData.Target = EnemyList[0];

		for (int i = 0; i < CurrentSquadMembers.Num(); i++)
		{
			AOxiAICharacter* const SquadMember = Cast<AOxiAICharacter>(CurrentSquadMembers[i]);
			if (SquadMember == nullptr)
			{
				continue;
			}

			SquadMember->IssueAICommand(AICommandData);
		}
	}

	for (int iSquad = 0; iSquad < CurrentSquadMembers.Num(); iSquad++)
	{
		AOxiAICharacter* const SquadMember = Cast<AOxiAICharacter>(CurrentSquadMembers[iSquad]);
		if (SquadMember == nullptr)
		{
			continue;
		}

		// TODO: Use Find and acquire here?
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
				
			SquadMember->AcquireCover(NearestCover);	// TODO - What if AI overrides?
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

/**
 *
 */
void UOxiAIManager::RegisterCover(AOxiCover* const Squad)
{
	CoverList.Add(Squad);
}

/**
 *
 */
void UOxiAIManager::UnregisterCover(AOxiCover* const Squad)
{
	CoverList.Remove(Squad);
}

/**
 *
 */
void UOxiAIManager::RegisterPlayer(AOxiFirstPersonCharacter* const Player)
{
	PlayerList.Add(Player);
}

/**
 *
 */
void UOxiAIManager::UnregisterPlayer(AOxiFirstPersonCharacter* const Player)
{
	PlayerList.Remove(Player);
}
