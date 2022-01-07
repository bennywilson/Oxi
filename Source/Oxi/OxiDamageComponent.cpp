// ELP 2020

#include "OxiDamageComponent.h"

UOxiDamageComponent::UOxiDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOxiDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Children = GetOwner()->GetComponentsByClass(USkeletalMeshComponent::StaticClass());

	for (int iChild = 0; iChild < Children.Num(); iChild++)
	{
		USkeletalMeshComponent* const SkelMesh = Cast<USkeletalMeshComponent>(Children[iChild]);
		if (SkelMesh == nullptr || SkelMesh->IsVisible() == false)
		{
			continue;
		}

		SkeletalMeshes.Add(SkelMesh);
	}
}

// Called every frame
void UOxiDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			FVector VecTo = GetOwner()->GetActorLocation() - PC->GetPawn()->GetActorLocation();
			VecTo.Z = 0.f;
			VecTo.Normalize();
			GetOwner()->SetActorRotation(VecTo.Rotation());
		}
	}*/
}

void UOxiDamageComponent::TakeDamage(const FOxiDamageInfo& DamageInfo)
{
	TakeDamage_Internal(DamageInfo);
}

void UOxiDamageComponent::DisableRagdoll()
{
	for (int i = 0; i < SkeletalMeshes.Num(); i++)
	{
		USkeletalMeshComponent* const SkelMesh = SkeletalMeshes[i];
		SkelMesh->PutAllRigidBodiesToSleep();
	}

	GetOwner()->GetWorldTimerManager().ClearTimer(RagdollSleepTimerHandle);
}