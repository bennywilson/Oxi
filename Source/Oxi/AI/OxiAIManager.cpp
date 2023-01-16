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

const int g_TileWidth = 16 * 3;
const int g_HalfTileWidth = g_TileWidth / 2;
const int g_NumCellsAcross = 3;
const int g_HalfNumCellsAcross = g_NumCellsAcross / 2;

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

	// Line Traces
	UpdateLineTraces();

	// Debug Drawing
	DrawDebugInfo();

	// Debug Drawing
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
void UOxiAIManager::UpdateLineTraces()
{
	if (CoverList.Num() == 0)
	{
		return;
	}

	if (CurrentCoverToTrace >= CoverList.Num())
	{
		CurrentCoverToTrace = 0;
	}

	AOxiCover* const currCover = CoverList[CurrentCoverToTrace];
	FVector basePos = PlayerList[0]->GetActorLocation();
	int xPos = ((int)basePos.X);
	xPos = xPos - (xPos % g_TileWidth);

	int yPos = ((int)basePos.Y);
	yPos = yPos - (yPos % g_TileWidth);

	const TArray<UOxiCoverSpotComponent*>& coverSpots = currCover->GetCoverSpots();
}

/**
 *
 */
void UOxiAIManager::DrawDebugInfo()
{
	if (PlayerList.Num() == 0)
	{
		return;
	}

	FVector basePos = PlayerList[0]->GetActorLocation();
	int xPos = ((int)basePos.X);
	xPos = xPos - (xPos % g_TileWidth);

	int yPos = ((int)basePos.Y);
	yPos = yPos - (yPos % g_TileWidth);

	for (int y = yPos - g_TileWidth * g_HalfNumCellsAcross; y <= yPos + g_TileWidth * g_HalfNumCellsAcross; y += g_TileWidth)
	{
		for (int x = xPos - g_TileWidth * g_HalfNumCellsAcross; x <= xPos + g_TileWidth * g_HalfNumCellsAcross; x += g_TileWidth)
		{
			DrawDebugBox(GetWorld(), FVector(x, y, basePos.Z), FVector(g_HalfTileWidth, g_HalfTileWidth, 0), FColor::Green, false, -1.0f, 0, 0.35f);
		}
	}

	// Debug Draw
	const int32 coverDebugLevel = CVarCoverDebug.GetValueOnGameThread();
	const int32 coverSpotDebugLevel = CVarCoverSpotDebug.GetValueOnGameThread();

	if (coverDebugLevel >= 1 && coverSpotDebugLevel >= 0)
	{
		bool drawLineTraces = coverDebugLevel == 0;
		AOxiCover* coverToDrawFrom = nullptr;
		for (int32 i = 0; i < CoverList.Num(); i++)
		{
			if (CoverList[i]->GetCoverIndex() == coverDebugLevel)
			{
				coverToDrawFrom = CoverList[i];
				break;
			}
		}

		if (coverToDrawFrom != nullptr)
		{
			const TArray<UOxiCoverSpotComponent*>& coverSpots = coverToDrawFrom->GetCoverSpots();
			if (coverSpotDebugLevel < coverSpots.Num())
			{
				TArray<AActor*> actorsToIgnore;
				actorsToIgnore.Add(PlayerList[0]);

				const UOxiCoverSpotComponent* const debugCoverSpot = coverSpots[coverSpotDebugLevel];
				for (int y = yPos - g_TileWidth * g_HalfNumCellsAcross; y <= yPos + g_TileWidth * g_HalfNumCellsAcross; y += g_TileWidth)
				{
					for (int x = xPos - g_TileWidth * g_HalfNumCellsAcross; x <= xPos + g_TileWidth * g_HalfNumCellsAcross; x += g_TileWidth)
					{
						FHitResult hitResult;
						FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
						params.AddIgnoredActors(actorsToIgnore);

						GetWorld()->LineTraceSingleByChannel(hitResult, debugCoverSpot->GetLeanLeftFirePoint(), FVector(x, y, basePos.Z), ECollisionChannel::ECC_Visibility, params);

						if (hitResult.bBlockingHit)
						{
							DrawDebugLine(GetWorld(), debugCoverSpot->GetLeanLeftFirePoint(), hitResult.ImpactPoint, FColor::Red, false, -1.0, 0, 0.32f);
						}
						else
						{
							DrawDebugLine(GetWorld(), debugCoverSpot->GetLeanLeftFirePoint(), FVector(x, y, basePos.Z), FColor::Blue, false, -1.0, 0, 0.32f);
						}
					}
				}
			}
		}
	}
}
