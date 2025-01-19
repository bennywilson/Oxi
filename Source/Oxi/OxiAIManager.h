// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "OxiCharacter.h"
#include "OxiCover.h"

#include "OxiAIManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOxiAI, Warning, All);

extern TAutoConsoleVariable<int32> CVarCoverDebug;
extern TAutoConsoleVariable<int32> CVarCoverSpotDebug;
extern TAutoConsoleVariable<int32> CVarEnableAI;

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
	Strafe,
	Idle,
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FOxiAICommandData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiAICommand AICommand = EOxiAICommand::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Target = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetPosition = FVector(0.0f, 0.0f, 0.0f);
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
	bool IssueFutureAICommand(const FOxiAICommandData& CommandData, const float secondsInTheFuture);

	UFUNCTION(BlueprintCallable)
	bool HasReachedDestination();

	UFUNCTION(BlueprintCallable)
	FVector GetPathDestination();

	UFUNCTION(BlueprintCallable)
	AOxiCover* FindAndAcquireCover(AActor* const attacker, const FVector searchLocation = FVector::ZeroVector, const float searchRadius = 0.0f);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	AActor* GetEnemyTarget();

protected:
	virtual void OnCoverProtectionLevelChanged(AOxiCover* const, EOxiCoverProtectionLevel) override;

	UFUNCTION()
	void DoFutureAICommandCallback(const FOxiAICommandData& CommandData);

protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	FOxiAICommandData CurrentAICommand;

	FTimerDelegate FutureAICommandTimerDel;
	FTimerHandle FutureAICommandTimerHandle;
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

	UFUNCTION(BlueprintCallable)
	void FindCoverWithinRadius(TArray<AOxiCover*>& cover, const FVector& searchCenter, const float radius);

	TArray<AOxiCover*>& GetCoverList() { return CoverList; }

	uint32 GetNextCoverIndex() const { return NextCoverIndex; }

	uint32 GetCurrentCoverVisTest() const { return CurrentCoverVisTest; }

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
	void DrawDebugInfo();

	void DebugDrawPlayerCells();

	int32 NextCoverIndex = 1;
	int32 CurrentCoverVisTest = 1;

public:
	const static int VisCellWidth = 16 * 3;
	const static int HalfVisCellWidth = VisCellWidth / 2;
	const static int VisCellsAcross = 4;
	const static int HalfVisCellsAcross = VisCellsAcross / 2;
};

extern UOxiAIManager * GetOxiAIManager(UObject* const WorldContextObject);