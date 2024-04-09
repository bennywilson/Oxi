#include "SRadio.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ASRadio::ASRadio()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp; //Set Root to the Mesh

	Counter = 0; //Number of Songs on Radio? (Needs to be edited to fit game)

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudoComp");
}
void ASRadio::BeginPlay()
{
	Super::BeginPlay();
}

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

	AudioComp->Stop();

	AudioComp->SetSound(SoundWaves[Counter]);

	AudioComp->Play();

	Counter++;


	UE_LOG(LogTemp, Warning, TEXT("%d"), Counter);
}

