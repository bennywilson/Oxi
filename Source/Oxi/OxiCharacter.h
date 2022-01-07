// ELP 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OxiWeapon.h"
#include "OxiDamageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OxiCharacter.generated.h"

class UInputComponent;

UENUM(BlueprintType)
enum OxiPlayerState
{
	Normal,
	Interacting
};

UCLASS()
class OXI_API UOxiAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxi Anim")
	TMap<FName, UAnimSequence*> AnimSequenceMap;
};

UCLASS()
class UOxiCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed);
};

UCLASS()
class AOxiCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOxiCharacter();
	AOxiCharacter(const FObjectInitializer&);
};

UCLASS()
class AOxiFirstPersonCharacter : public AOxiCharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UFUNCTION(BlueprintCallable, Category = "Oxi Character")
	void ChangeOxiPlayerState(OxiPlayerState NewPlayerState, FVector InteractionPos, bool MovePlayer, FRotator InteractionRot, bool RotatePlayer);

	UFUNCTION(BlueprintCallable, Category = "Oxi Character")
	OxiPlayerState GetOxiPlayerState() const { return PlayerState; }

	UPROPERTY(Transient, BlueprintReadOnly)
	AOxiWeapon* EquippedItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<AOxiWeapon> DefaultWeapon;

public:
	AOxiFirstPersonCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction);

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BaseHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FName GripPointName;

	UPROPERTY(BlueprintReadWrite, interp, Category = Light)
	FLinearColor OxiColor;

	UPROPERTY(BlueprintReadWrite, interp, Category = Light)
	FLinearColor BloodColor;

	UPROPERTY(BlueprintReadWrite, interp, Category = Light)
	FLinearColor OxiLightColor;

	UPROPERTY(BlueprintReadWrite, interp, Category = Light)
	FLinearColor BloodLightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxi Damage")
	TArray<ULightComponent*> OxiPulseLightList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VO")
	TArray<class USoundBase*> EnemyKilledVO;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon")
	bool StartFireWeapon(const UCameraComponent* const CameraComp1P);

private:

	UMaterialInstanceDynamic* HandsMaterial;
	float CurrentHealth;

private:
	virtual float TakeDamage_Internal(const FOxiDamageInfo& DamageInfo);

protected:

	void OnFire();

	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:

	UFUNCTION()
	void OnCharacterDeathEvent(class UOxiDamageComponent* Victim, UOxiDamageComponent* Killer);

	FDelegateHandle OnCharacterDeathEventHandle;

	// Player State	
	OxiPlayerState PlayerState;

	float CurT;
	FVector StartLocation;
	FVector TargetLocation;
	bool ShouldMoveTowardsTarget;

	FRotator StartRotation;
	FRotator TargetRotation;
	bool ShouldRotateTowardsTarget;
};