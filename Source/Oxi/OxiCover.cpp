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

	if (coverDebug == 0 || coverDebug == CoverIndex)
	{
		static float debugSphereRadius = 10.0f;
		static float debugArrowLength = 20;
		static float debugArrowThickness = 2.0f;
		static float debugArrowSize = 15.0f;

		for (int i = 0; i < CoverSpotList.Num(); i++)
		{
			if (coverSpotDebug != 0 && coverSpotDebug != i)
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

			const FVector ShootHeight(0.0f, 0.0f, 170.0f);
			const float ShootRightDistFromCoverSpot = 55.0f;
			const float ShootForwardDistFromCoverSpot = 55.0f;

			const FVector LeftLeanShootPos = coverSpotLocation + ShootHeight + FVector(coverSpotDirection.Y, coverSpotDirection.X, 0.0f) * ShootRightDistFromCoverSpot + FVector(coverSpotDirection.X, coverSpotDirection.Y, 0.0f) * ShootForwardDistFromCoverSpot;
			const FVector RightLeanShootPos = coverSpotLocation + ShootHeight + FVector(-coverSpotDirection.Y, -coverSpotDirection.X, 0.0f) * ShootRightDistFromCoverSpot + FVector(coverSpotDirection.X, coverSpotDirection.Y, 0.0f) * ShootForwardDistFromCoverSpot;

			DrawDebugSphere(GetWorld(), LeftLeanShootPos, debugSphereRadius, 16, FColor::Green);
			DrawDebugSphere(GetWorld(), RightLeanShootPos, debugSphereRadius, 16, FColor::Green);
		}
	}
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
