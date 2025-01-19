// ELP 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"
#include "OxiDamageComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FDamageComponentOnDeath, UOxiDamageComponent* const, class AActor* const, class AActor* const);


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
	TEnumAsByte<EOxiDamageType> DamageType = EOxiDamageType::Bullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DamageLocation = FVector(0.0, 0.0, 0.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactNormal = FVector(0.0, 0.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* DamageCauser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HitBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageXYImpulse = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageZImpulse = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamageComponentOnTakeDamage, AActor*, damagedActor, FOxiDamageInfo, damageInfo);

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
	float MaterialWoundRadius = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneConstraintToBreak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 ClipSphereLocationAndRadius = FVector4(0.0, 0.0, 0.0, 0.0);

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
	float BloodSprayDistance = 1.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OXI_API UOxiDamageComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	FDamageComponentOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FDamageComponentOnTakeDamage OnTakeDamage;

	// Sets default values for this component's properties
	UOxiDamageComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DisableRagdoll();

	UFUNCTION(BlueprintCallable, Category = "Oxi Character")
	virtual float TakeDamage(const FOxiDamageInfo& DamageInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = "Oxi Character")
	float TakeDamage_Internal(const FOxiDamageInfo& DamageInfo);

	UFUNCTION(BlueprintCallable, Category = "Oxi Character")
	bool IsAlive() const { return CurrentHealth > 0; }

protected:

	virtual void BeginPlay() override;

	void BroadcastDeath();

protected:

	TArray<USkeletalMeshComponent*>& GetSkeletalMeshes() { return SkeletalMeshes; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BaseHealth;

	UPROPERTY(Transient, BlueprintReadWrite)
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
