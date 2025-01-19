// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
//#include "Factories/Factory.h"
#include "OxiPhysicalMaterial.generated.h"
/*
UCLASS()
class OXI_API UOxiPhysicalMaterialFactory : public UFactory
{
	GENERATED_BODY()

public:

	UOxiPhysicalMaterialFactory();

	virtual uint32 GetMenuCategories() const override;


	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};*/

/**
 *
 */
UENUM(BlueprintType)
enum class EChippableType : uint8
{
	None,
	EntireMesh,
	PerCollisionShape,
};


/**
 * 
 */
UCLASS(BlueprintType)
class OXI_API UOxiPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ChippableDecalActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EChippableType ChippableType;
};
