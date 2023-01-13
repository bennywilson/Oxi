// ELP 2022

#include "OxiAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "OxiHumanDamageComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

DEFINE_LOG_CATEGORY(LogOxiAI);

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

/**
 *
 */
void UOxiAIManager::Tick(float DeltaTime)
{
	static int breakhere = 0;
	breakhere++;

	if (PlayerList.Num() == 0)
	{
		return;
	}
	const int tileWidth = 16;
	const int halfTileWidth = tileWidth / 2;
	const int NumCellsAcross = 8;
	const int HalfNumCellsAcross = NumCellsAcross / 2;
	FVector basePos = PlayerList[0]->GetActorLocation();
	int xPos = ((int)basePos.X);
	xPos = xPos - (xPos % tileWidth);

	int yPos = ((int)basePos.Y);
	yPos = yPos - (yPos % tileWidth);

	UE_LOG(LogTemp, Log, TEXT("[%d], [%d]"), xPos, yPos);

	for (int y = yPos - tileWidth * HalfNumCellsAcross; y <= yPos + tileWidth * HalfNumCellsAcross; y += tileWidth)
	{
		for (int x = xPos - tileWidth * HalfNumCellsAcross; x <= xPos + tileWidth * HalfNumCellsAcross; x += tileWidth)
		{
			DrawDebugBox(GetWorld(), FVector(x, y, basePos.Z), FVector(halfTileWidth, halfTileWidth, 0), FColor::Green, false, -1.0f, 0, 0.35f);

		}
	}

//	DrawDebugBox(GetWorld(), FVector(xPos, yPos, basePos.Z), FVector(halfTileWidt	h, halfTileWidth, halfTileWidth), FColor::Green, false);
}

/**
 *
 */
TStatId UOxiAIManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UOxiAIManager, STATGROUP_Tickables);
}
