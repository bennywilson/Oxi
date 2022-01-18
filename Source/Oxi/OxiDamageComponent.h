// ELP 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "OxiDamageComponent.generated.h"

UENUM(BlueprintType)
enum EOxiDamageType
{
	Bullet = 0,
	Explosion,
};

USTRUCT(BlueprintType)
struct FOxiDamageInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EOxiDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DamageLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageCauser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HitBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageXYImpulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageZImpulse;
};


USTRUCT(BlueprintType)
struct FWoundFXData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> DismembermentFX;

	UPROPERTY(EditAnywhere)
	TArray<FName> TagsOfComponentsToEnable;
};

USTRUCT(BlueprintType)
struct FWoundData
{
	GENERATED_BODY()

	FWoundData() :
		ClipSphereLocationAndRadius(0.0f, 0.0f, 0.0f, 0.0f) { }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaterialWoundRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneConstraintToBreak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 ClipSphereLocationAndRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWoundFXData WoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWoundFXData GibFX;
};

struct FWoundInstance
{
	FName BoneName;			// TODO: Remove as it's already in WoundData
	int WoundIndex;
	FVector HitLocation;
	float HitTime;
};

/**
 * 
 */
 USTRUCT(BlueprintType)
struct FOxiBloodSplatterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> SplatterActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BloodSprayDistance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OXI_API UOxiDamageComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOxiDamageComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DisableRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Oxi Character")
	virtual void TakeDamage(const FOxiDamageInfo& DamageInfo);

	bool IsAlive() const { return CurrentHealth > 0; }

protected:

	virtual void BeginPlay() override;

	// IOxiDamageInterface
protected:
	
	virtual float TakeDamage_Internal(const FOxiDamageInfo& DamageInfo) { return 0.0f; }

	TArray<USkeletalMeshComponent*>& GetSkeletalMeshes() { return SkeletalMeshes; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BaseHealth;

	UPROPERTY(Transient)
	float CurrentHealth;

	UPROPERTY(Transient)
	TArray<USkeletalMeshComponent*> SkeletalMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	TArray<FWoundData> WoundData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOxiBloodSplatterData> BloodSplatter;

	TArray<FWoundInstance> WoundInstances;

	FTimerHandle RagdollSleepTimerHandle;
};	
