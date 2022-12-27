// ELP 2022

#include "OxiSquad.h"
#include "Kismet/GameplayStatics.h"
#include "OxiAIManager.h"
#include "OxiHumanDamageComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

/** Whether to allow preshadows (static world casting on character), can be disabled for debugging. */
static TAutoConsoleVariable<int32> CVarSquadDebug(
	TEXT("oxi.squaddebug"),
	0,
	TEXT("Show debug squad indo"),
	ECVF_Cheat
);


/**
 * 
 */
AOxiSquad::AOxiSquad()
{
	PrimaryActorTick.bCanEverTick = true;
	TargetsPositionRadius = 1000.0f;
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
void AOxiSquad::SquadMemberKilledCB(UOxiDamageComponent* const DamageComp, AActor* const Victim, AActor* const Killer)
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
	switch (SquadState)
	{
		case EOxiSquadState::Idle :
		{
			TickIdleState(DeltaTime); 
			break;
		}

		case EOxiSquadState::Attack:
		{
			TickAttackState(DeltaTime);
			break;
		}
	}

	const int debugSquadLevel = CVarSquadDebug.GetValueOnGameThread();
	if (debugSquadLevel)
	{
		if (debugSquadLevel == 1 || debugSquadLevel == 2)
		{
			for (int i = 0; i < SquadTargets.Num(); i++)
			{
				const FOxiSquadTarget& curSquadTarget = SquadTargets[i];
				const FMatrix debugMatrix(FVector::UpVector, FVector::ForwardVector, FVector::LeftVector, curSquadTarget.Location);

				DrawDebugSolidCircle(GetWorld(), debugMatrix, TargetsPositionRadius, 36, FColor(255, 0, 0, 64), false, -1.0f, SDPG_World);
			}
		}

		if (debugSquadLevel == 1 || debugSquadLevel == 3)
		{
			for (int i = 0; i < CurrentSquadMembers.Num(); i++)
			{
				CurrentSquadMembers[i]->DebugDraw(debugSquadLevel);
			}
		}
	}
}

/**
 *
 */
void AOxiSquad::TickIdleState(const float DeltaTime)
{
	UOxiAIManager* AIMgr = GetOxiAIManager(this);
	TArray<AOxiFirstPersonCharacter*> PlayerList = AIMgr->GetPlayerList();
	if (PlayerList.Num() == 0)
	{
		return;
	}

	AOxiFirstPersonCharacter* const Player = PlayerList[0];
	for (int i = 0; i < CurrentSquadMembers.Num(); i++)
	{
		AOxiCharacter* const SquadMember = CurrentSquadMembers[i];
		if (PerceptionRadius <= 0.0f || FVector::Dist(SquadMember->GetActorLocation(), Player->GetActorLocation()) <= PerceptionRadius)
		{
			SquadTargets.Empty();
			FOxiSquadTarget NewTarget;
			NewTarget.Character = Player;
			NewTarget.Location = Player->GetActorLocation();
			SquadTargets.Add(NewTarget);

			EnterAttackState();
			break;
		}
	}
}

/**
 *
 */
void AOxiSquad::EnterAttackState()
{
	check(SquadTargets.Num() > 0);

	if (DefaultSquadActions.Num() == 0)
	{
		return;
	}

	SquadState = EOxiSquadState::Attack;

	TSubclassOf<UOxiSquadAction> DesiredAction;
	if (DebugAction.Get() != nullptr)
	{
		DesiredAction = DebugAction;
	}
	else
	{
		DesiredAction = DefaultSquadActions[FMath::RandRange(0, DefaultSquadActions.Num() - 1)];
	}

	CurrentAction = NewObject<UOxiSquadAction>(this, DesiredAction);
	CurrentAction->StartAction(this);
}

/**
 *
 */
void AOxiSquad::TickAttackState(const float DeltaTime)
{
	for (int i = 0; i < SquadTargets.Num(); i++)
	{
		FOxiSquadTarget& CurTarget = SquadTargets[i];
		const FVector ActorsLocation = CurTarget.Character->GetActorLocation();
		if (FVector::Dist(ActorsLocation, CurTarget.Location) > TargetsPositionRadius)
		{
			const FVector OldPosition = CurTarget.Location;
			CurTarget.Location = ActorsLocation;			
			CurrentAction->OnTargetChangedPosition(CurTarget, OldPosition);
			break;
		}
	}

	check(CurrentAction != nullptr);
	CurrentAction->TickAction(DeltaTime);
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
void UOxiSquadAction::GetCoverInRadius(TArray<AOxiCover*>& OutCoverList, const FVector& TestPoint, const float radius)
{
	UOxiAIManager* const OxiMgr = GetOxiAIManager(this);
	OutCoverList = OxiMgr->GetCoverList();
}