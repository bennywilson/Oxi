// ELP 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnProtectionlevelSignature, class AOxiCover* const, EOxiCoverProtectionLevel);

/**
 * 
 */
UCLASS(Blueprintable)
class OXI_API AOxiCover : public AActor
{
	GENERATED_BODY()
	
public:

	AOxiCover();

	EOxiCoverProtectionLevel GetCoverProtectionLevel() const { return ProtectionLevel;}

	FOnProtectionlevelSignature ProtectionLevelChanged;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UOxiDestructibleComponent* DestructibleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOxiCoverProtectionLevel ProtectionLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* UndamagedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* DamagedMesh;
};
