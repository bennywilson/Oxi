// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiAIManager.h"
#include "GameFramework/PhysicsVolume.h"
#include "OxiAISpawnSquadTrigger.generated.h"

/**
 * 
 */
UCLASS()
class OXI_API AAISquadMemberSpawn : public AActor
{
	GENERATED_BODY()

public:
	AAISquadMemberSpawn(const FObjectInitializer&);

	TSubclassOf<AOxiCharacter> GetOxiCharacterClassToSpawn() const { return OxiCharacterToSpawn; }

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AOxiCharacter> OxiCharacterToSpawn;

#if WITH_EDITORONLY_DATA
	/* Reference to the billboard component */
	UPROPERTY()
	UBillboardComponent* SpriteComponent;

	UPROPERTY()
	UArrowComponent* ArrowComponent;
#endif
};

UENUM()
enum class EAISpawnSquadTriggerCondition
{
	PlayerEntersVolume = 0,
};

/**
 * 
 */
UCLASS()
class OXI_API AAISpawnSquadTrigger : public APhysicsVolume
{
	GENERATED_BODY()
	
	virtual void ActorEnteredVolume(class AActor* Other) override;

private:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AOxiSquad>> SquadTypes;

	UPROPERTY(EditAnywhere)
	TArray<AAISquadMemberSpawn*> SquadMembersToSpawn;

	UPROPERTY()
	EAISpawnSquadTriggerCondition SpawnCondition;

	UPROPERTY(Transient)
	class AOxiSquad* Squad;
};
