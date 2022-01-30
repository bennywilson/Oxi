// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiAIManager.h"
#include "GameFramework/PhysicsVolume.h"
#include "OxiSquad.h"
#include "OxiAIVolumes.generated.h"

/**
 * 
 */
UCLASS()
class OXI_API AOxiAISquadMemberSpawn : public AActor
{
	GENERATED_BODY()

public:
	AOxiAISquadMemberSpawn(const FObjectInitializer&);

	TSubclassOf<AOxiCharacter> GetOxiCharacterClassToSpawn() const { return OxiCharacterToSpawn; }

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AOxiCharacter> OxiCharacterToSpawn;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UBillboardComponent* SpriteComponent;

	UPROPERTY()
	UArrowComponent* ArrowComponent;
#endif
};

/**
 * 
 */
UENUM()
enum class EAISpawnSquadTriggerCondition
{
	PlayerEntersVolume = 0,
};

/**
 *
 */
UCLASS()
class OXI_API AOxiBattlefieldVolume : public APhysicsVolume
{
	 GENERATED_BODY()
};

/**
 * 
 */
UCLASS()
class OXI_API AOxiAISpawnSquadTrigger : public APhysicsVolume
{
	GENERATED_BODY()
	
	virtual void ActorEnteredVolume(class AActor* Other) override;

private:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AOxiSquad>> SquadTypes;

	UPROPERTY(EditAnywhere)
	TArray<AOxiAISquadMemberSpawn*> SquadMembersToSpawn;

	UPROPERTY()
	EAISpawnSquadTriggerCondition SpawnCondition;

	// Volume which encompasses the battle field the squad is in.  If not specified, the trigger volume will be used
	UPROPERTY(EditAnywhere)
	AOxiBattlefieldVolume* BattlefieldVolume;

	// Actions squad will take if the other lists are empty or their conditions haven't been met
	UPROPERTY(EditAnywhere)
	TArray<UOxiSquadAction*> DefaultSquadActions;

	// Actions squad will take at the beginning of battle
	UPROPERTY(EditAnywhere)
	TArray<UOxiSquadAction*> InitialSquadActions;

	// Actions squad will take when desperate (ex. losing the fight, low morale, etc)
	UPROPERTY(EditAnywhere)
	TArray<UOxiSquadAction*> DesperateSquadActions;

	// Actions squad will take when winning (ex. player is hurt, high morale, etc)
	UPROPERTY(EditAnywhere)
	TArray<UOxiSquadAction*> ConfidentSquadActions;

	UPROPERTY(Transient)
	class AOxiSquad* Squad;
};
