// ELP 2023

#include "OxiCheckpoint.h"

AOxiCheckpoint::AOxiCheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComponent);
}

// Called when the game starts or when spawned
void AOxiCheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOxiCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

