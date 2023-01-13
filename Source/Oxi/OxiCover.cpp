// ELP 2022

#include "OxiCover.h"
#include "OxiDestructibleComponent.h"
#include "AI/OxiAIManager.h"

/** Toggles onscreen AI debugging information */
static TAutoConsoleVariable<int32> CVarSquadDebug(
	TEXT("oxi.coverdebug"),
	0,
	TEXT("Show debug cover info"),
	ECVF_Cheat
);

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
	GetOxiAIManager(this)->RegisterCover(this);

	TArray<UActorComponent*> CoverSpots = GetComponentsByClass(UOxiCoverSpotComponent::StaticClass());
	for (int i = 0; i < CoverSpots.Num(); i++)
	{
		CoverSpotList.Add(static_cast<UOxiCoverSpotComponent*>(CoverSpots[i]));
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

	if (CVarSquadDebug.GetValueOnAnyThread() > 0)
	{
		static float debugSphereRadius = 10.0f;
		static float debugArrowLength = 20;
		static float debugArrowThickness = 2.0f;
		static float debugArrowSize = 15.0f;

		for (int i = 0; i < CoverSpotList.Num(); i++)
		{
			FVector ComponentLocation = CoverSpotList[i]->GetComponentLocation();
			DrawDebugSphere(GetWorld(), ComponentLocation, debugSphereRadius, 16, FColor::Green);
			DrawDebugDirectionalArrow(GetWorld(), ComponentLocation, ComponentLocation + CoverSpotList[i]->GetComponentRotation().Vector() * debugArrowLength, debugArrowSize, FColor::Red, false, -1.0f, 0, debugArrowThickness);
		}
	}
}

/**
 *
 */
bool AOxiCover::AddUser(AOxiCharacter* const NewUser)
{
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
UOxiCoverSpotComponent::UOxiCoverSpotComponent(FObjectInitializer const& objecttInitializer)
	: UArrowComponent(objecttInitializer)
{

}
