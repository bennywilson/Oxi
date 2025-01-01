// Fill out your copyright notice in the Description page of Project Settings.

/*
#include "OxiPhysicalMaterial.h"
#include "AssetTypeCategories.h"

UOxiPhysicalMaterialFactory::UOxiPhysicalMaterialFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	//Configure the class that this factory creates
	SupportedClass = UOxiPhysicalMaterial::StaticClass();
}

uint32 UOxiPhysicalMaterialFactory::GetMenuCategories() const
{
	//Let's place this asset in the Blueprints category in the Editor
	return EAssetTypeCategories::Blueprint;
}

UObject* UOxiPhysicalMaterialFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	//Create the editor asset 
	UOxiPhysicalMaterial* PhysMat = NewObject<UOxiPhysicalMaterial>(InParent, InClass, InName, Flags);
	return PhysMat;
}*/