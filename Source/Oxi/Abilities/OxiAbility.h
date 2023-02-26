// ELP 2023


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OxiAbility.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OXI_API UOxiAbility : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOxiAbility();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
