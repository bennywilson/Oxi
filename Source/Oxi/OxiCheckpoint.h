// ELP 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OxiCheckpoint.generated.h"

UCLASS(Blueprintable)
class OXI_API AOxiCheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOxiCheckpoint();

	void ReloadCheckpoint(ACharacter* const Player);

protected:
	UPROPERTY(EditAnywhere)
	TArray<class ATriggerBox*> TriggersToActivateOnReload;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;
#endif // WITH_EDITORONLY_DATA
};
