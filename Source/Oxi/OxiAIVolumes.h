// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiAIManager.h"
#include "OxiSquad.h"
#include "GameFramework/PhysicsVolume.h"
#include "OxiAIVolumes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSquadSpawned, class AOxiCharacter* const, character);

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
	
	AOxiAISpawnSquadTrigger();

	virtual void ActorEnteredVolume(class AActor* Other) override;

	UFUNCTION(BlueprintCallable)
	void ActivateSpawnSquadTrigger();

protected:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AOxiSquad>> SquadTypes;

	UPROPERTY(EditAnywhere)
	TArray<AOxiAISquadMemberSpawn*> SquadMembersToSpawn;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TArray<AOxiAICharacter*> PosedSquadMembersToSpawn;

	UPROPERTY()
	EAISpawnSquadTriggerCondition SpawnCondition;

	UPROPERTY(EditAnywhere)
	struct FOxiSquadBehaviorContexts BehaviorOverrides;

	UPROPERTY(Transient)
	class AOxiSquad* Squad;

	UPROPERTY(Transient)
	bool HasFired;

	UPROPERTY(BlueprintAssignable)
	FOnSquadSpawned OnSquadSpawned;
};
