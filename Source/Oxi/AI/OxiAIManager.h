// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiCover.h"

#include "OxiAIManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOxiAI, Warning, All);

extern TAutoConsoleVariable<int32> CVarCoverDebug;
extern TAutoConsoleVariable<int32> CVarCoverSpotDebug;

/**
 *
 */
UENUM(BlueprintType)
enum class EOxiAICommand : uint8
{
	None,
	TakeCover,
	RushTarget,
	FlankTarget,
	HoldPosition,
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FOxiAICommandData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiAICommand AICommand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Goal;
};

/**
 *
 */
UCLASS(BlueprintType)
class OXI_API AOxiAICharacter : public AOxiCharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IssueAICommand(const FOxiAICommandData& CommandData);

	UFUNCTION(BlueprintCallable)
	bool HasReachedDestination();

	UFUNCTION(BlueprintCallable)
	AOxiCover* FindAndAcquireCover();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	AActor* GetEnemyTarget();

protected:
	virtual void OnCoverProtectionLevelChanged(AOxiCover* const, EOxiCoverProtectionLevel) override;

protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	FOxiAICommandData CurrentAICommand;
};

/**
 *
 */
UCLASS()
class OXI_API UOxiAIManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void RegisterSquad(class AOxiSquad* const Squad);
	void UnregisterSquad(AOxiSquad* const Squad);

	uint32 RegisterCover(AOxiCover* const Cover);
	void UnregisterCover(AOxiCover* const Cover);

	void RegisterPlayer(AOxiFirstPersonCharacter* const Player);
	void UnregisterPlayer(AOxiFirstPersonCharacter* const Player);

	const TArray<AOxiFirstPersonCharacter*> GetPlayerList() const { return PlayerList; }

	UFUNCTION(BlueprintCallable)
	AOxiCover* FindNearestUnusedCover(const FVector& TestPoint);

	TArray<AOxiCover*>& GetCoverList() { return CoverList; }

protected:
	UPROPERTY(Transient)
	TArray<AOxiFirstPersonCharacter*> PlayerList;

	UPROPERTY(Transient)
	TArray<AOxiSquad*> SquadList;

	UPROPERTY(Transient, BlueprintReadOnly)
	TArray<AOxiCover*> CoverList;

	int32 CurrentCoverToTrace;

	// FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

private:
	void UpdateLineTraces();

	void DrawDebugInfo();

	uint32 NextCoverIndex = 1;

public:
	const static int TileWidth = 16 * 3;
	const static int HalfTileWidth = TileWidth / 2;
	const static int NumCellsAcross = 3;
	const static int HalfNumCellsAcross = NumCellsAcross / 2;
};

extern UOxiAIManager * GetOxiAIManager(UObject* const WorldContextObject);