// ELP 2022

#include "OxiCover.h"
#include "OxiDestructibleComponent.h"
#include "AI/OxiAIManager.h"


/**
 *
 */
void UOxiCoverSpotComponent::BeginPlay()
{
	Super::BeginPlay();

	const FVector ShootHeight(0.0f, 0.0f, 170.0f);
	const float ShootRightDistFromCoverSpot = 55.0f;
	const float ShootForwardDistFromCoverSpot = 55.0f;

	const FVector coverSpotLocation = GetComponentLocation();
	const FVector coverSpotDirection = GetComponentRotation().Vector();
	LeanLeftWorldFirePoint = coverSpotLocation + ShootHeight + FVector(coverSpotDirection.Y, coverSpotDirection.X, 0.0f) * ShootRightDistFromCoverSpot + FVector(coverSpotDirection.X, coverSpotDirection.Y, 0.0f) * ShootForwardDistFromCoverSpot;
	LeanRightWorldFirePoint = coverSpotLocation + ShootHeight + FVector(-coverSpotDirection.Y, -coverSpotDirection.X, 0.0f) * ShootRightDistFromCoverSpot + FVector(coverSpotDirection.X, coverSpotDirection.Y, 0.0f) * ShootForwardDistFromCoverSpot;

	LeftLeanVisibilityToTarget = 0;
	RightLeanVisibilityToTarget = 0;
}

/**
 *
 */
AOxiCover::AOxiCover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	//SetRootComponent(NewSceneComponent);

	DestructibleComponent = CreateDefaultSubobject<UOxiDestructibleComponent>(TEXT("DestructibleComponent"));
	//DestructibleComponent->SetupAttachment(NewSceneComponent);
	SetRootComponent(DestructibleComponent);
		
	UndamagedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UndamagedMesh"));
	UndamagedMesh->SetupAttachment(DestructibleComponent);

	DamagedMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DamagedMesh"));
	DamagedMesh->SetupAttachment(DestructibleComponent);
	DamagedMesh->SetCollisionProfileName(FName("BlockAllDynamic"));
}

/**
 *
 */
void AOxiCover::BeginPlay()
{
	Super::BeginPlay();
	
	DestructibleComponent->InitDestructibleComponent(UndamagedMesh, DamagedMesh);

	DestructibleComponent->OnTakeDamageDelegate.AddUObject(this, &AOxiCover::OnDestructibleTakeDamage);
	CoverIndex = GetOxiAIManager(this)->RegisterCover(this);

	for (int i = 0; i < CoverSpotList.Num(); i++)
	{
		UOxiCoverSpotComponent* const coverSpot = Cast<UOxiCoverSpotComponent>(CoverSpotList[i].GetComponent(this));
		if (coverSpot == nullptr)
		{
			continue;
		}

		CoverSpots.Add(coverSpot);
	}
}

/**
 *
 */
void AOxiCover::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	if (AIMgr != nullptr)
	{
		AIMgr->UnregisterCover(this);
	}
	DestructibleComponent->OnTakeDamageDelegate.RemoveAll(this);
}

/**
 *
 */
void AOxiCover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const int coverDebug = CVarCoverDebug.GetValueOnGameThread();
	const int coverSpotDebug = CVarCoverSpotDebug.GetValueOnGameThread();
	const bool bDebugDraw = coverDebug == 0 || coverDebug == CoverIndex;

	if (bDebugDraw)
	{
		static float debugSphereRadius = 10.0f;
		static float debugArrowLength = 20;
		static float debugArrowThickness = 2.0f;
		static float debugArrowSize = 15.0f;

		for (int i = 0; i < CoverSpotList.Num(); i++)
		{
			if (coverSpotDebug != 0 && coverSpotDebug != i + 1)
			{
				continue;
			}

			UOxiCoverSpotComponent* const coverSpot = Cast<UOxiCoverSpotComponent>(CoverSpotList[i].GetComponent(this));
			if (coverSpot == nullptr)
			{
				continue;
			}

			const FVector coverSpotLocation = coverSpot->GetComponentLocation();
			const FVector coverSpotDirection = coverSpot->GetComponentRotation().Vector();

			DrawDebugSphere(GetWorld(), coverSpotLocation, debugSphereRadius, 16, FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), coverSpotLocation, coverSpotLocation + coverSpotDirection * debugArrowLength, debugArrowSize, FColor::Red, false, -1.0f, 0, debugArrowThickness);

			if (coverDebug == 0)
			{
				continue;
			}

			FColor sphereColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, coverSpot->GetLeftLeanVisibilityToTarget()).ToFColor(false);
			DrawDebugSphere(GetWorld(), coverSpot->GetLeanLeftFirePoint(), debugSphereRadius, 16, sphereColor);

			sphereColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, coverSpot->GetRightLeanVisibilityToTarget()).ToFColor(false);
			DrawDebugSphere(GetWorld(), coverSpot->GetLeanRightFirePoint(), debugSphereRadius, 16, sphereColor);
		}
	}

	UOxiAIManager* const aiMgr = GetOxiAIManager(this);
	auto PlayerList = aiMgr->GetPlayerList();
	if (PlayerList.Num() == 0)
	{
		return;
	}

	FVector basePos = PlayerList[0]->GetActorLocation();
	int xPos = ((int)basePos.X);
	xPos = xPos - (xPos % UOxiAIManager::TileWidth);

	int yPos = ((int)basePos.Y);
	yPos = yPos - (yPos % UOxiAIManager::TileWidth);

	for (int y = yPos - UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; y <= yPos + UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; y += UOxiAIManager::TileWidth)
	{
		for (int x = xPos - UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; x <= xPos + UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; x += UOxiAIManager::TileWidth)
		{
			DrawDebugBox(GetWorld(), FVector(x, y, basePos.Z), FVector(UOxiAIManager::HalfTileWidth, UOxiAIManager::HalfTileWidth, 0), FColor::Green, false, -1.0f, 0, 0.35f);
		}
	}

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(PlayerList[0]);

	const int32 visTileWidth = UOxiAIManager::TileWidth;
	const int32 halfNumCellsAcross = UOxiAIManager::HalfNumCellsAcross;

	// Get Last Frame
	for (int iCoverSpot = 0; iCoverSpot < CoverSpotList.Num(); iCoverSpot++)
	{
		UOxiCoverSpotComponent* const curCoverSpot = CoverSpots[iCoverSpot];
		auto& visiblityTraces = curCoverSpot->GetVisibilityHandles();
		const int32 halfNumTraces = visiblityTraces.Num() / 2;

		float leftLeanVis = 0;
		float rightLeanVis = 0;
		for (int32 iTrace = 0; iTrace < visiblityTraces.Num(); iTrace++)
		{
			FTraceDatum traceData;
			const bool bTraceValid = GetWorld()->QueryTraceData(visiblityTraces[iTrace], traceData);
			if (bTraceValid == false)
			{
				continue;
			}

			if (traceData.OutHits.Num() == 0)
			{
				if (iTrace < halfNumTraces)
				{
					leftLeanVis += 1.0f;
				}
				else
				{
					rightLeanVis += 1.0f;
				}
			}
	
			if (bDebugDraw && (coverSpotDebug == 0 || coverSpotDebug - 1 == iCoverSpot))
			{
				if (traceData.OutHits.Num() > 0)
				{
					DrawDebugLine(GetWorld(), traceData.Start, traceData.OutHits[0].ImpactPoint, FColor::Red, false, -1.0, 0, 0.32f);
				}
				else
				{
					DrawDebugLine(GetWorld(), traceData.Start, traceData.End, FColor::Blue, false, -1.0, 0, 0.32f);
				}
			}
		}

		curCoverSpot->SetLeftLeanVisibilityToTarget(leftLeanVis / halfNumTraces);
		curCoverSpot->SetRightLeanVisibilityToTarget(rightLeanVis / halfNumTraces);
		visiblityTraces.Empty();
	}

	// Queue up async line traces
	for (int iCoverSpot = 0; iCoverSpot < CoverSpotList.Num(); iCoverSpot++)
	{
		UOxiCoverSpotComponent* const curCoverSpot = CoverSpots[iCoverSpot];

		float leftLeanVis = 0;
		float rightLeanVis = 0;

		const FVector firePositions[] = { curCoverSpot->GetLeanLeftFirePoint(), curCoverSpot->GetLeanRightFirePoint() };

		for (int iLean = 0; iLean < 2; iLean++)
		{
			for (int y = yPos - visTileWidth * halfNumCellsAcross; y <= yPos + visTileWidth * halfNumCellsAcross; y += visTileWidth)
			{
				for (int x = xPos - visTileWidth * halfNumCellsAcross; x <= xPos + visTileWidth * halfNumCellsAcross; x += visTileWidth)
				{
					FHitResult hitResult;
					FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
					params.AddIgnoredActors(actorsToIgnore);
					const FVector cellPos = FVector(x, y, basePos.Z);

					FTraceHandle traceHandle = GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, firePositions[iLean], cellPos, ECollisionChannel::ECC_Visibility, params);
					if (traceHandle.IsValid() == false)
					{
						continue;
					}

					curCoverSpot->GetVisibilityHandles().Add(traceHandle);
					//DrawDebugLine(GetWorld(), firePositions[iLean], cellPos, FColor::Red, false, -1.0, 0, 0.32f);
				}
			}
		}
	}

	/*float leftPointVis = 0, rightPointVis = 0;

	for (int iTrace = 0; iTrace < VisibilityTraceHandles.Num(); iTrace++)
	{
		FTraceDatum traceData;
		const bool bTraceValid = GetWorld()->QueryTraceData(VisibilityTraceHandles[iTrace], traceData);
		if (bTraceValid == false)
		{
			continue;
		}

		if (traceData.OutHits.Num() > 0)
		{
			if (iTrace < halfTraceListSize)
			{
				leftPointVis += 1.0f;
			}
			else
			{
				rightPointVis += 1.0f;
			}
		}

		if (bDebugDraw)
		{
			if (traceData.OutHits.Num() == 0)
			{
				DrawDebugLine(GetWorld(), traceData.Start, traceData.End, FColor::Blue, false, -1.0, 0, 0.32f);
			}
			else
			{
				leftPointVis += 1.0f;
				DrawDebugLine(GetWorld(), traceData.Start, traceData.OutHits[0].ImpactPoint, FColor::Red, false, -1.0, 0, 0.32f);
			}
		}
	}
	leftPointVis /= halfWayIdx;
	rightPointVis /= halfWayIdx;
	

	VisibilityTraceHandles.Empty();

	if (coverDebugLevel >= 1 && coverSpotDebugLevel >= 0)
	{
		TArray<AOxiCover*>& coverList = aiMgr->GetCoverList();
		bool drawLineTraces = coverDebugLevel == 0;
		AOxiCover* coverToDrawFrom = nullptr;
		for (int32 i = 0; i < coverList.Num(); i++)
		{
			if (coverList[i]->GetCoverIndex() == coverDebugLevel)
			{
				coverToDrawFrom = coverList[i];
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
				for (int y = yPos - UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; y <= yPos + UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; y += UOxiAIManager::TileWidth)
				{
					for (int x = xPos - UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; x <= xPos + UOxiAIManager::TileWidth * UOxiAIManager::HalfNumCellsAcross; x += UOxiAIManager::TileWidth)
					{
						FHitResult hitResult;
						FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
						params.AddIgnoredActors(actorsToIgnore);
						const FVector cellPos = FVector(x, y, basePos.Z);
						//GetWorld()->LineTraceSingleByChannel(hitResult, debugCoverSpot->GetLeanLeftFirePoint(), cellPos, ECollisionChannel::ECC_Visibility, params);
						FTraceHandle traceHandle = GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, debugCoverSpot->GetLeanLeftFirePoint(), cellPos, ECollisionChannel::ECC_Visibility, params);
						if (traceHandle.IsValid() == false)
						{
							continue;
						}

						VisibilityTraceHandles.Add(traceHandle);
					}
				}
			}
		}
	}*/
}

/**
 *
 */
bool AOxiCover::AddUser(AOxiCharacter* const NewUser)
{
	// Only one user per cover for now
	check(CurrentUsers.Num() == 0);
	
	CurrentUsers.Add(NewUser);
	OnProtectionLevelChanged.AddUObject(NewUser, &AOxiCharacter::OnCoverProtectionLevelChanged);

	return true;
}

/**
 * 
 */
void AOxiCover::RemoveUser(AOxiCharacter* const UserToRemove)
{
	CurrentUsers.Remove(UserToRemove);
	OnProtectionLevelChanged.RemoveAll(UserToRemove);
}

/**
 *
 */
void AOxiCover::OnDestructibleTakeDamage(AActor* const DamageCauser, float DamageAmount)
{
	if (DestructibleComponent->GetNumBrokenPieces() == -1)
	{
		ProtectionLevel = EOxiCoverProtectionLevel::Broken;
		OnProtectionLevelChanged.Broadcast(this, EOxiCoverProtectionLevel::Broken);
	}
}

/**
 *
 */
UOxiCoverSpotComponent* AOxiCover::GetBestCoverSpot(const FVector TargetPosition)
{
	const FVector CoverToTargetVec = (TargetPosition - GetActorLocation()).GetSafeNormal();
	float closestVecMatch = -999.0f;
	UOxiCoverSpotComponent* BestCoverSpot = nullptr;
	for (int i = 0; i < CoverSpotList.Num(); i++)
	{
		UOxiCoverSpotComponent* const CurCoverSpot = Cast<UOxiCoverSpotComponent>(CoverSpotList[i].GetComponent(this));
		float dot = CurCoverSpot->GetComponentRotation().Vector().Dot(CoverToTargetVec);
		if (dot > closestVecMatch)
		{
			BestCoverSpot = CurCoverSpot;
			closestVecMatch = dot;
		}
	}

	return BestCoverSpot;
}

/**
 *
 */
UOxiCoverSpotComponent::UOxiCoverSpotComponent(FObjectInitializer const& objecttInitializer)
	: UArrowComponent(objecttInitializer)
{

}
