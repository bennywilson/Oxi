// Fill out your copyright notice in the Description page of Project Settings.


#include "SRadio.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASRadio::ASRadio()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp; //Set Root to the Mesh

	Counter = 5; //Number of Songs on Radio? (Needs to be edited to fit game)
}

// Called when the game starts or when spawned
void ASRadio::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASRadio::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASRadio::Interact_Implementation(APawn* InstigatorPawn)
{
	if (Counter >= SoundWaves.Num())
	{
		Counter = 0;
	}

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundWaves[Counter], GetActorLocation(), 1.0f, 1.0f);

	Counter++;
}

