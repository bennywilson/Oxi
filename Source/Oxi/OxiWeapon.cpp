// ELP 2020

#include "OxiWeapon.h"
#include "PhysicsEngine/BodySetup.h"

/**
 *
 */
AOxiWeapon::AOxiWeapon()
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponComponent"));
	SetRootComponent(Mesh1P);
	Mesh1P->SetOnlyOwnerSee(false);
	//Mesh1P->SetupAttachment(RootComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	//Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

FName AOxiWeapon::GetBodyName(const UStaticMeshComponent* const StaticMeshComp, const int ElementIndex)
{
	if (ElementIndex < 0 || ElementIndex >= StaticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.BoxElems.Num())
	{
		return NAME_None;
	}

	return StaticMeshComp->GetStaticMesh()->GetBodySetup()->AggGeom.BoxElems[ElementIndex].GetName();
}