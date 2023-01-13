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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProtectionLevel, class AOxiCover* const, EOxiCoverProtectionLevel);

UCLASS(Blueprintable, editinlinenew, meta = (BlueprintSpawnableComponent))
class OXI_API UOxiCoverSpotComponent : public UArrowComponent
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiCoverProtectionLevel Bleh;
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

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
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

	TArray<UOxiCoverSpotComponent*> CoverSpotList;

private:
	UPROPERTY(Transient)
	TArray<AOxiCharacter*> CurrentUsers;
};
