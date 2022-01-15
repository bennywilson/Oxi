// ELP 2022

#include "OxiAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "OxiHumanDamageComponent.h"

DEFINE_LOG_CATEGORY(LogOxiAI);

/**
 *	AI can ignore the AI command
 */
bool AOxiAICharacter::IssueSquadCommand_Implementation(const OxiAICommand AICommand, const FOxiAICommandData&)
{
	CurrentAICommand = AICommand;
	return true;
}

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
					SquadState = EOxiSquadState::Attack;
				}
			}
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
