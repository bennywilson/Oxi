// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SRadio.generated.h"

class UStaticMeshComponent;
class USoundWave;
class UAudioComponent;

UCLASS()
class OXI_API ASRadio : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

	void Interact_Implementation(APawn* InstigatorPawn);//Expected for using native. 
	
public:	
	// Sets default values for this actor's properties
	ASRadio();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComp; //Mesh For Radio

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> SoundWaves;

	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly)
	int Counter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
