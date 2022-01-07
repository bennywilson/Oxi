// ELP 2020

#include "OxiCharacter.h"
#include "OxiProjectile.h"
#include "OxiGameMode.h"
#include "OxiWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

void UOxiCharacterMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	Super::RequestDirectMove(MoveVelocity, bForceMaxSpeed);
}

AOxiCharacter::AOxiCharacter()
{

}

AOxiCharacter::AOxiCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UOxiCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

//////////////////////////////////////////////////////////////////////////
// AOxiFirstPersonCharacter

AOxiFirstPersonCharacter::AOxiFirstPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	PlayerState = OxiPlayerState::Normal;
}

void AOxiFirstPersonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FName PlayerSocketToAttachTo = "RightHandAttachSocket";

	EquippedItem = GWorld->SpawnActor<AOxiWeapon>(DefaultWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
	EquippedItem->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, PlayerSocketToAttachTo);
	EquippedItem->SetActorRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	Mesh1P->SetHiddenInGame(false, true);

	HandsMaterial = Mesh1P->CreateDynamicMaterialInstance(0);
	HandsMaterial->SetVectorParameterValue("PulseColor", OxiColor);

	for (int i = 0; i < OxiPulseLightList.Num(); i++)
	{
		OxiPulseLightList[i]->SetLightColor(OxiLightColor);
	}
//	HandsMaterial->SetVectorParameterValue("PulseColor2", OxiColor);

	CurrentHealth = BaseHealth;

	OnCharacterDeathEventHandle = UCombatManager::RegisterEventListener(this, FName("OnCharacterDeathEvent"));
}

void AOxiFirstPersonCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	UCombatManager::UnregisterEventListener(OnCharacterDeathEventHandle);
}

void AOxiFirstPersonCharacter::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (PlayerState == OxiPlayerState::Interacting)
	{

		CurT = FMath::Clamp(CurT + DeltaTime, 0.0f, 1.0f);
		if (ShouldMoveTowardsTarget)
		{
			const float MoveT = FMath::Clamp(CurT * 3.0f, 0.0f, 1.0f);
			const FVector CurActorLocation = GetActorLocation();
			const FVector NewActorLocation = FMath::Lerp(StartLocation, TargetLocation, FMath::Clamp(MoveT, 0.0f, 1.0f));
			FirstPersonCameraComponent->SetWorldLocation(NewActorLocation);
		}

		if (ShouldRotateTowardsTarget)
		{
			const float RotT = FMath::Clamp(CurT * 3.0f, 0.0f, 1.0f);
			const FQuat CurActorRot = FQuat(GetActorRotation());
			const FQuat NewActorRot = FMath::Lerp(FQuat(StartRotation), FQuat(TargetRotation), RotT);

			FirstPersonCameraComponent->SetWorldRotation(NewActorRot.Rotator(), false, nullptr, ETeleportType::None);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AOxiFirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AOxiFirstPersonCharacter::OnFire);


	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AOxiFirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AOxiFirstPersonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AOxiFirstPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AOxiFirstPersonCharacter::LookUpAtRate);
}

void AOxiFirstPersonCharacter::OnFire()
{
	StartFireWeapon(FirstPersonCameraComponent);
}

void AOxiFirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AOxiFirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AOxiFirstPersonCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOxiFirstPersonCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

float AOxiFirstPersonCharacter::TakeDamage_Internal(const FOxiDamageInfo& DamageInfo)
{
	CurrentHealth -= DamageInfo.DamageAmount;
	
	if (CurrentHealth < 0.0f)
	{
		CurrentHealth = 0.0f;
	}
	const float t = 1.0f - ((float)CurrentHealth / BaseHealth);

	FLinearColor CurBloodColor = FMath::Lerp(OxiColor, BloodColor, t);

	HandsMaterial->SetVectorParameterValue("PulseColor", CurBloodColor);
	//HandsMaterial->SetVectorParameterValue("PulseColor2", OxiColor);

	for (int i = 0; i < OxiPulseLightList.Num(); i++)
	{
		OxiPulseLightList[i]->SetLightColor(CurBloodColor);
	}
	return 0.f;
}

void AOxiFirstPersonCharacter::OnCharacterDeathEvent(UOxiDamageComponent* Victim, UOxiDamageComponent* Killer)
{
	if (EnemyKilledVO.Num() > 0)
	{
//		const int idx = FMath::Rand() % EnemyKilledVO.Num();
		static int curIdx = 0;
		UGameplayStatics::PlaySoundAtLocation(this, EnemyKilledVO[curIdx], FirstPersonCameraComponent->GetComponentToWorld().GetLocation());
		curIdx++;
		if (curIdx >= EnemyKilledVO.Num())
		{
			curIdx = 0;
		}
	}
}

void AOxiFirstPersonCharacter::ChangeOxiPlayerState(OxiPlayerState InPlayerState, FVector InteractionPos, bool bMovePlayer, FRotator InteractionRot, bool bRotatePlayer)
{
	PlayerState = InPlayerState;
	ShouldMoveTowardsTarget = bMovePlayer;
	if (ShouldMoveTowardsTarget)
	{
		TargetLocation = InteractionPos;
	}

	ShouldRotateTowardsTarget = bRotatePlayer;
	if (ShouldRotateTowardsTarget)
	{
		TargetRotation = InteractionRot;
	}

	if (PlayerState == OxiPlayerState::Interacting)
	{
		GetCharacterMovement()->SetActive(false);
		GetController()->SetIgnoreMoveInput(true);
		GetController()->SetIgnoreLookInput(true);

		StartLocation = FirstPersonCameraComponent->GetComponentLocation();
		StartRotation = FirstPersonCameraComponent->GetComponentRotation();
		FirstPersonCameraComponent->bUsePawnControlRotation = false;
		CurT = 0;
	}
	else
	{

		GetCharacterMovement()->SetActive(true);
		GetController()->SetIgnoreMoveInput(false);
		GetController()->SetIgnoreLookInput(false);
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
	}
}
