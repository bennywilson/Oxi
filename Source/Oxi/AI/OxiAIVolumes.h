// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiAIManager.h"
#include "OxiSquad.h"
#include "GameFramework/PhysicsVolume.h"
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

	UFUNCTION(BlueprintCallable)
	void ActivateSpawnSquadTrigger();

private:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AOxiSquad>> SquadTypes;

	UPROPERTY(EditAnywhere)
	TArray<AOxiAISquadMemberSpawn*> SquadMembersToSpawn;

	UPROPERTY()
	EAISpawnSquadTriggerCondition SpawnCondition;

	UPROPERTY(EditAnywhere)
	struct FOxiSquadBehaviorContexts BehaviorOverrides;

	UPROPERTY(Transient)
	class AOxiSquad* Squad;
};
