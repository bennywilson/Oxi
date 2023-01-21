// ELP 2022

#include "OxiAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "OxiHumanDamageComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogOxiAI);

/** Toggles onscreen AI debugging information */
TAutoConsoleVariable<int32> CVarCoverDebug(
	TEXT("oxi.coverdebug"),

	-1,
	TEXT("Show debug cover info. 0 Shows all.  Otherwise the # indicates a specific cover to display"),
	ECVF_Cheat
);

TAutoConsoleVariable<int32> CVarCoverSpotDebug(
	TEXT("oxi.coverspotdebug"),
	-1,
	TEXT("Show debug cover spot info. 0 Shows all.  Otherwise the # indicates a specific cover spor to display"),
	ECVF_Cheat
);

/**
 *
 */
UOxiAIManager* GetOxiAIManager(UObject* const WorldContextObject)
{
	UGameInstance* const GameInst = UGameplayStatics::GetGameInstance(WorldContextObject);
	check(GameInst);

	return GameInst->GetSubsystem<UOxiAIManager>();
}

/**
 *
 */
AOxiCover* UOxiAIManager::FindNearestUnusedCover(const FVector& TestPoint)
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

		const float CoverDist = FVector::Dist(CurrentCover->GetActorLocation(), TestPoint);
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
	check(AIController);

	const UPathFollowingComponent* const PathComponent = AIController->GetPathFollowingComponent();
	return PathComponent->DidMoveReachGoal();
}

/**
 * 
 */
AOxiCover* AOxiAICharacter::FindAndAcquireCover()
{
	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	AOxiCover* const NewCover = AIMgr->FindNearestUnusedCover(GetActorLocation());
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
			AICommandData.AICommand = EOxiAICommand::TakeCover;
			AICommandData.Target = CurrentAICommand.Target;
			AICommandData.Goal = NearestCover;

			IssueAICommand(AICommandData);
		}
	}
}

/**
 *
 */
void UOxiAIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentCoverToTrace = 0;
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
uint32 UOxiAIManager::RegisterCover(AOxiCover* const Squad)
{
	CoverList.Add(Squad);
	
	uint32 coverIdx = NextCoverIndex;
	NextCoverIndex++;
	return coverIdx;
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

/**
 *
 */
void UOxiAIManager::Tick(float DeltaTime)
{
	if (PlayerList.Num() == 0)
	{
		return;
	}

	CurrentCoverVisTest++;
	if (CurrentCoverVisTest > CoverList.Num())
	{
		CurrentCoverVisTest = 1;
	}

	// Debug Drawing
	DrawDebugInfo();

	DebugDrawPlayerCells();
}

/**
 *
 */
TStatId UOxiAIManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UOxiAIManager, STATGROUP_Tickables);
}

/**
 *
 */
void UOxiAIManager::DrawDebugInfo()
{
}

/**
 *
 */
void UOxiAIManager::DebugDrawPlayerCells()
{
	if (PlayerList.Num() == 0)
	{
		return;
	}

	// Draw Player Cells
	const FVector basePos = PlayerList[0]->GetActorLocation();
	const int xIdx = ((int)basePos.X);
	const int yIdx = ((int)basePos.Y);

	const float startOffset = -((VisCellWidth * HalfVisCellsAcross) - (VisCellWidth * 0.5f));
	
	for (int y = 0; y < VisCellsAcross; y++)
	{
		const float yPos = basePos.Y + startOffset + (y * VisCellWidth);
		for (int x = 0; x < VisCellsAcross; x++)
		{
			const float xPos = basePos.X + startOffset + (x * VisCellWidth);

			UE_LOG(LogTemp, Log, TEXT("[%f %f %f] -- [%f %f %f]"), basePos.X, basePos.Y, basePos.Z, xPos, yPos, basePos.Z);

			DrawDebugBox(GetWorld(), FVector(xPos, yPos, basePos.Z), FVector(HalfVisCellWidth, HalfVisCellWidth, 0), FColor::Green, false, -1.0f, 0, 0.35f);
		}
	}
}