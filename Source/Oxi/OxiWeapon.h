// ELP 2020

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OxiWeapon.generated.h"

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class OXI_API AItem : public AActor
{
	GENERATED_BODY()

};

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class OXI_API AOxiWeapon : public AItem
{
	GENERATED_BODY()

public:

	AOxiWeapon();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Animations")
	bool StartFireWeapon(const UCameraComponent* const FirstPersonCameraComponent, const bool bIsOutline, const FVector TargetLocation);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int ClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int CurrentAmmoCount;
};