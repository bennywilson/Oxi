// ELP 2022

#include "AIManager.h"
#include "Components/BillboardComponent.h"
#include "AISpawnSquadTrigger.h"

AAISquadMemberSpawn::AAISquadMemberSpawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* NewSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(NewSceneComponent);

#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> DecalTexture;
			FName ID_SpawnSquadTrigger;
			FText NAME_Decals;
			FConstructorStatics()
				: DecalTexture(TEXT("/Game/Oxi/Editor/S_HiddenHand"))
				, ID_SpawnSquadTrigger(TEXT("SpawnSquad"))
				, NAME_Decals(NSLOCTEXT("SpriteCategory", "SpawnSquad", "SpawnSquads"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		if (SpriteComponent)
		{
			SpriteComponent->Sprite = ConstructorStatics.DecalTexture.Get();
			SpriteComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_SpawnSquadTrigger;
			SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Decals;
			SpriteComponent->SetupAttachment(NewSceneComponent);
			SpriteComponent->bIsScreenSizeScaled = true;
			SpriteComponent->SetUsingAbsoluteScale(true);
			SpriteComponent->bReceivesDecals = false;
		}
	}
#endif
}

void AAISpawnSquadTrigger::ActorEnteredVolume(class AActor* Other)
{
	AOxiFirstPersonCharacter* const Player = Cast <AOxiFirstPersonCharacter>(Other);
	if (Other == nullptr)
	{
		return;
	}

	if (Squad != nullptr && Squad->GetNumAliveSquadMembers() > 0)
	{
		return;
	}

	if (SquadMembersToSpawn.Num() == 0)
	{
		UE_LOG(LogOxiAI, Warning, TEXT("AAISpawnSquadTrigger::ActorEnteredVolume() - Volume %s has no squad members to spawn"), *GetFullName());
		return;
	}

	if (Squad != nullptr)
	{
		Squad->ConditionalBeginDestroy();
		Squad = nullptr;
	}

	Squad = NewObject<UOxiSquad>();
	for (int i = 0; i < SquadMembersToSpawn.Num(); i++)
	{
		if (SquadMembersToSpawn[i] == nullptr)
		{
			UE_LOG(LogOxiAI, Warning, TEXT("AAISpawnSquadTrigger::ActorEnteredVolume() - Volume %s has no null squad member entries"), *GetFullName());
			continue;
		}

		TSubclassOf<AOxiCharacter> ActorToSpawn = SquadMembersToSpawn[i]->GetOxiCharacterClassToSpawn();

		AOxiAICharacter* const OxiChar = Cast<AOxiAICharacter>(GWorld->SpawnActor(ActorToSpawn, &SquadMembersToSpawn[i]->GetTransform()));
		check(OxiChar != nullptr);

		Squad->AddSquadMember(OxiChar);
	}
}