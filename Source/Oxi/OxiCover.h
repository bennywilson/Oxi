// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "OxiCover.generated.h"

/**
 *
 */
UENUM()
enum class EOxiCoverProtectionLevel
{
	Unbreakable,
	High,
	Medium,
	Low,
	Broken
};

/**
 *
 */
struct FDebugTraceData
{
	FVector Start;
	FVector End;
	bool HitSomething;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProtectionLevel, class AOxiCover* const, EOxiCoverProtectionLevel);

UCLASS(Blueprintable, editinlinenew, meta = (BlueprintSpawnableComponent))
class OXI_API UOxiCoverSpotComponent : public UArrowComponent
{
	friend class AOxiCover;

	GENERATED_UCLASS_BODY()

	virtual void BeginPlay() override;

	FVector GetLeanLeftFirePoint() const { return LeanLeftWorldFirePoint; }
	FVector GetLeanRightFirePoint() const { return LeanRightWorldFirePoint; }

	void SetLeftLeanVisibilityToTarget(const float visibility) { LeftLeanVisibilityToTarget = visibility; }
	void SetRightLeanVisibilityToTarget(const float visibility) { RightLeanVisibilityToTarget = visibility; }

	UFUNCTION(BlueprintCallable)
	float GetLeftLeanVisibilityToTarget() const { return LeftLeanVisibilityToTarget; }

	UFUNCTION(BlueprintCallable)
	float GetRightLeanVisibilityToTarget() const { return RightLeanVisibilityToTarget; }

	TArray<FDebugTraceData>& GetDebugTraceDataList() { return DebugTraceDataList; }

protected:

	TArray<FTraceHandle>& GetVisibilityHandles() { return VisibilityTraceHandles; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiCoverProtectionLevel Bleh;

private:
	FVector LeanLeftWorldFirePoint;
	FVector LeanRightWorldFirePoint;

	TArray<FDebugTraceData> DebugTraceDataList;

	TArray<FTraceHandle> VisibilityTraceHandles;
	float LeftLeanVisibilityToTarget;
	float RightLeanVisibilityToTarget;
};

/**
 * 
 */
UCLASS(Blueprintable)
class OXI_API AOxiCover : public AActor
{
	GENERATED_BODY()
	
public:
	AOxiCover();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool AddUser(AOxiCharacter* const NewUser);

	UFUNCTION(BlueprintCallable)
	void RemoveUser(AOxiCharacter* const UserToRemove);

	int GetNumUsers() const { return CurrentUsers.Num(); }

	EOxiCoverProtectionLevel GetCoverProtectionLevel() const { return ProtectionLevel;}

	UFUNCTION(BlueprintCallable)
	UOxiCoverSpotComponent* GetBestCoverSpot(const FVector TargetPosition);

	const TArray<UOxiCoverSpotComponent*>& GetCoverSpots() const { return CoverSpots; }

	uint32 GetCoverIndex() const { return CoverIndex; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnDestructibleTakeDamage(AActor* const DamageCauser, float DamageAmount);

public:
	FOnProtectionLevel OnProtectionLevelChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UOxiDestructibleComponent* DestructibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiCoverProtectionLevel ProtectionLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* UndamagedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* DamagedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FComponentReference> CoverSpotList;

	// At init, the components in CoverSpotList are moved into CoverSpots
	TArray<UOxiCoverSpotComponent*> CoverSpots;

private:

	void UpdateVisTraces();

	void DebugDrawCoverInfo();

	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentUsers;

	uint32 CoverIndex;

	FScriptDelegate TakeDamageDelegate;
};
