// ELP 2020

#include "OxiSentryDamageComponent.h"
#include "OxiSentryCharAnimInstance.h"
#include "OxiGameMode.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "Kismet/GameplayStatics.h"

void UOxiSentryDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);

	for (int i = 0; i < Children.Num(); i++)
	{
		USkeletalMeshComponent* const SkelMesh = Cast<USkeletalMeshComponent>(Children[i]);
		if (SkelMesh == nullptr)
		{
			continue;
		}
		/*
		SkelMesh->SetSimulatePhysics(false);
		SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkelMesh->SetAllBodiesPhysicsBlendWeight(0.f);*/
	}

	LastClipTime = -999999.0f;
}


void UOxiSentryDamageComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			if (PC == nullptr || PC->GetPawn() == nullptr)
			{
				continue;
			}
			FVector VecTo = PC->GetPawn()->GetActorLocation() - GetOwner()->GetActorLocation();
			VecTo.Normalize();
			GetOwner()->SetActorRotation(VecTo.Rotation());
		}
	}

	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);

	const float UnpausedTimeSec = GetWorld()->GetUnpausedTimeSeconds();
	for (int i = 0; i < Children.Num(); i++)
	{
		USkeletalMeshComponent* const SkelMesh = Cast<USkeletalMeshComponent>(Children[i]);
		if (SkelMesh == nullptr)
		{
			continue;
		}

		for (int MatIdx = 0; MatIdx < SkelMesh->GetNumMaterials(); MatIdx++)
		{
			UMaterialInstanceDynamic* const DynMat = Cast<UMaterialInstanceDynamic>(SkelMesh->GetMaterial(MatIdx));
			if (DynMat == nullptr)
			{
				continue;
			}

			static FName ClipBoneParams[] = { "ClipBone1Params", "ClipBone2Params", "ClipBone3Params" };
			static FName ClipBoneGlowParams[] = { "ClipBone1GlowParams", "ClipBone2GlowParams", "ClipBone3GlowParams" };
			for (int ClipBoneIdx = 0; ClipBoneIdx < WoundInstances.Num(); ClipBoneIdx++)
			{
				FLinearColor ClipBoneParam;
				ClipBoneParam.R = WoundInstances[ClipBoneIdx].HitLocation.X;
				ClipBoneParam.G = WoundInstances[ClipBoneIdx].HitLocation.Y;
				ClipBoneParam.B = WoundInstances[ClipBoneIdx].HitLocation.Z;
				ClipBoneParam.A = 5.0f;
				DynMat->SetVectorParameterValue(ClipBoneParams[ClipBoneIdx], ClipBoneParam);

				const float GlowIntensity = 1.0f - FMath::Clamp((UnpausedTimeSec - WoundInstances[ClipBoneIdx].HitTime) / 1.0f, 0.0f, 1.0f);
				DynMat->SetScalarParameterValue(ClipBoneGlowParams[ClipBoneIdx], GlowIntensity);
			}
		}

		/*if (BaseHealth > 0)
		{
			for (int BodyIdx = 0; BodyIdx < SkelMesh->Bodies.Num(); BodyIdx++)
			{
				FBodyInstance* const BodyInst = SkelMesh->Bodies[BodyIdx];

				static float Magnitude = 0.01f;
				FVector RandImpulse(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));
				RandImpulse *= Magnitude;
				BodyInst->AddImpulse(RandImpulse, true);
			}
		}*/
	}
}


float UOxiSentryDamageComponent::TakeDamage(const FOxiDamageInfo& DamageInfo)
{
	Super::TakeDamage_Internal(DamageInfo);
	
	float DamageAmount = DamageInfo.DamageAmount;
	bool KillShot = true;

	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);

	for (int i = 0; i < Children.Num(); i++)
	{
		USkeletalMeshComponent* const SkelMesh = Cast<USkeletalMeshComponent>(Children[i]);
		if (SkelMesh == nullptr)
		{
			continue;
		}

		static FName RootBone("Bone");

		bool bCanClipBone = DamageInfo.HitBoneName != RootBone;
		bool bBoneClipped = false;
		if (bCanClipBone)
		{
			for (int ClipBoneIdx = 0; ClipBoneIdx < WoundInstances.Num(); ClipBoneIdx++)
			{
				if (SkelMesh->BoneIsChildOf(WoundInstances[ClipBoneIdx].BoneName, DamageInfo.HitBoneName))
				{
					bCanClipBone = false;
					break;
				}
			}
		}

		const float UnpausedTimeSec = GetWorld()->GetUnpausedTimeSeconds();
		if (bCanClipBone && WoundInstances.Num() < 3 && GetWorld()->GetUnpausedTimeSeconds() > LastClipTime + 0.5f)
		{
			LastClipTime = GetWorld()->GetUnpausedTimeSeconds();
			const int32 HitBoneIdx = SkelMesh->GetBoneIndex(DamageInfo.HitBoneName);

			if (HitBoneIdx != INDEX_NONE)
			{
				//const FVector RefBonePos = SkelMesh->GetRefPosePosition(HitBoneIdx);
				FWoundInstance HitInfo;
				HitInfo.HitLocation = SkelMesh->GetBoneLocation(DamageInfo.HitBoneName, EBoneSpaces::ComponentSpace);
				HitInfo.BoneName = DamageInfo.HitBoneName;
				HitInfo.HitTime = UnpausedTimeSec;
				WoundInstances.Add(HitInfo);

				DamageAmount *= 0.5f;
				SkelMesh->SetAllBodiesBelowSimulatePhysics(DamageInfo.HitBoneName, true);
				SkelMesh->SetAllBodiesBelowPhysicsBlendWeight(DamageInfo.HitBoneName, 1.0f);

				// Break the constraint
				int32 ConstraintIndex = SkelMesh->FindConstraintIndex(DamageInfo.HitBoneName);
				if (ConstraintIndex != INDEX_NONE && ConstraintIndex < SkelMesh->Constraints.Num())
				{
					FConstraintInstance* Constraint = SkelMesh->Constraints[ConstraintIndex];
					// If already broken, our job has already been done. Bail!
					if (Constraint->IsTerminated() == false)
					{
						UPhysicsAsset* const PhysicsAsset = SkelMesh->GetPhysicsAsset();
						FBodyInstance* Body = SkelMesh->GetBodyInstance(Constraint->JointName);

						if (Body != NULL && !Body->IsInstanceSimulatingPhysics())
						{
							Body->SetInstanceSimulatePhysics(true);
						}

						// Break Constraint
						Constraint->TermConstraint();
						bBoneClipped = true;
					}
				}

				static float Scalar = 1000.0f;
				const FVector Impulse = (DamageInfo.DamageLocation - DamageInfo.DamageCauser->GetActorLocation()).GetSafeNormal() * Scalar;
				SkelMesh->AddImpulse(Impulse, DamageInfo.HitBoneName, true);
			}

			for (int MatIdx = 0; MatIdx < SkelMesh->GetNumMaterials(); MatIdx++)
			{
				UMaterialInstanceDynamic* const DynMat = Cast<UMaterialInstanceDynamic>(SkelMesh->GetMaterial(MatIdx));
				if (DynMat == nullptr)
				{
					continue;
				}

				static FName ClipBoneParams[] = {"ClipBone1Params", "ClipBone2Params", "ClipBone3Params"};
				for (int ClipBoneIdx = 0; ClipBoneIdx < WoundInstances.Num(); ClipBoneIdx++)
				{
					FLinearColor ClipBoneParam;
					ClipBoneParam.R = WoundInstances[ClipBoneIdx].HitLocation.X;
					ClipBoneParam.G = WoundInstances[ClipBoneIdx].HitLocation.Y;
					ClipBoneParam.B = WoundInstances[ClipBoneIdx].HitLocation.Z;
					ClipBoneParam.A = 5.0f;
					DynMat->SetVectorParameterValue(ClipBoneParams[ClipBoneIdx], ClipBoneParam);
				}
			}
		}

		if (BaseHealth > 1 || (DamageInfo.HitBoneName != NAME_None && DamageInfo.HitBoneName != RootBone))
		{
			//KillShot = false;	// TODO HACK
		}

		if (BaseHealth <= 0)
		{
			return 0.f;
		}

		const bool JustKilled = KillShot && (BaseHealth > 0 && (BaseHealth - DamageInfo.DamageAmount) <= 0.f);

		BaseHealth -= DamageAmount;

		if (BaseHealth <= 0 && KillShot == false)
		{
			BaseHealth = 1.0f;
		}

		UOxiSentryCharAnimInstance* const AnimInstance = Cast<UOxiSentryCharAnimInstance>(SkelMesh->GetAnimInstance());
		if (JustKilled)
		{
			const float GibRoll = FMath::FRand();

			const FVector OurLocation = GetComponentTransform().GetLocation();
			if (GibRoll < GibChance && GibList.Num() > 0)
			{
				GetOwner()->Destroy();
				const int GibIdx = FMath::RandRange(0, GibList.Num() - 1);
				GetWorld()->SpawnActor(GibList[GibIdx], &GetComponentTransform());
			}
			else
			{
				SkelMesh->SetSimulatePhysics(true);
				SkelMesh->SetAllBodiesPhysicsBlendWeight(1.f);

				if (AnimInstance != nullptr)
				{
					AnimInstance->PlayHitReaction(DamageInfo.DamageAmount, DamageInfo.DamageLocation, DamageInfo.DamageCauser, false, false);
					AnimInstance->PlayDeathReaction(DamageInfo.DamageAmount, DamageInfo.DamageLocation, DamageInfo.DamageCauser);
				}

				FVector Impulse = (OurLocation - DamageInfo.DamageLocation).GetSafeNormal();
				static float Scalar = 1000.0f;
				Impulse *= Scalar;
				SkelMesh->AddImpulse(Impulse, NAME_None, true);
			}

			GetWorld()->GetTimerManager().SetTimer(DeleteTimer, this, &UOxiSentryDamageComponent::LifeSpanCallback, 3.0f, false);
			DeathStartTime = UGameplayStatics::GetTimeSeconds(GetWorld());

			UCombatManager::TriggerDeathEvent(this, nullptr);
		}
		else
		{
			if (AnimInstance != nullptr)
			{
				AnimInstance->PlayHitReaction(DamageInfo.DamageAmount, DamageInfo.DamageLocation, DamageInfo.DamageCauser, bCanClipBone, bBoneClipped);
			}
		}
	}
	return 0.f;
}

void UOxiSentryDamageComponent::LifeSpanCallback()
{
	DestroyComponent();
	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}