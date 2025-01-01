// ELP 2020

#include "OxiCharacter.h"
#include "OxiGameMode.h"
#include "OxiWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "OxiHumanDamageComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/LightComponent.h"
#include "GameFramework/InputSettings.h"
#include "SGameplayInterface.h"
#include "OxiAIManager.h"
#include "OxiCheatManager.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

void UOxiCharacterMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	Super::RequestDirectMove(MoveVelocity, bForceMaxSpeed);
}

/**
 *
 */
AOxiCharacter::AOxiCharacter()
{
}

/**
 *
 */
AOxiCharacter::AOxiCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UOxiCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
}

/**
 * 
 */
void AOxiCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UOxiDamageComponent* const DamageComp = Cast<UOxiDamageComponent>(GetComponentByClass(UOxiDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnDeath.AddUObject(this, &AOxiCharacter::OnDeath);
	}
}

/**x
 *
 */
void AOxiCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UOxiHumanDamageComponent* const DamageComp = Cast<UOxiHumanDamageComponent>(GetComponentByClass(UOxiHumanDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		DamageComp->OnDeath.RemoveAll(this);
	}

	ReleaseCover();
}


/**
 *
 */
void AOxiCharacter::OnDeath(UOxiDamageComponent* const DamageComp, AActor* const Victim, AActor* const Killer)
{
	OnDeath_Internal(Cast<UOxiHumanDamageComponent>(DamageComp), Victim, Killer);

	check(DamageComp);
	DamageComp->OnDeath.RemoveAll(this);

	ReleaseCover();

	FTimerHandle TearDownTimerHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(TearDownTimerHandle, this, &AOxiCharacter::DestroyCharacter, 10.0f, false);
}

/**
 *
 */
void AOxiCharacter::DestroyCharacter()
{

}

/**
 *
 */
bool AOxiCharacter::AcquireCover(AOxiCover* const cover, const FVector threatLocation)
{
	if (cover->AddUser(this))
	{
		CurrentCover = cover;

		check(CurrentCover);

		CurrentCoverSpot = cover->GetBestCoverSpot(threatLocation);
		return true;
	}

	return false;
}

/**
 * 
 */
void AOxiCharacter::ReleaseCover()
{
	if (CurrentCover == nullptr)
	{
		return;
	}

	CurrentCover->RemoveUser(this);
	CurrentCover = nullptr;
}

/**
 * 
 */
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

/**
 *
 */
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

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	AIMgr->RegisterPlayer(this);

	UOxiDamageComponent* const DamageComp = Cast<UOxiDamageComponent>(GetComponentByClass(UOxiDamageComponent::StaticClass()));
	if (DamageComp != nullptr)
	{
		//DamageComp->OnTakeDamage.Add()
//AddUObject(this, &AOxiFirstPersonCharacter::DamageTakenCB);
	}


	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

/**
 *
 */
void AOxiFirstPersonCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
	UCombatManager::UnregisterEventListener(OnCharacterDeathEventHandle);

	UOxiAIManager* const AIMgr = GetOxiAIManager(this);
	AIMgr->UnregisterPlayer(this);
}

/**
 * 
 */
void AOxiFirstPersonCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

/**
 *
 */
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

	if (IsFireDown)
	{
		StartFireWeapon(FirstPersonCameraComponent);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AOxiFirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOxiFirstPersonCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOxiFirstPersonCharacter::Look);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AOxiFirstPersonCharacter::OnStartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AOxiFirstPersonCharacter::OnStopFire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AOxiFirstPersonCharacter::TryReload);
		EnhancedInputComponent->BindAction(SwapWeaponAction, ETriggerEvent::Triggered, this, &AOxiFirstPersonCharacter::TrySwitchWeapon);

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AOxiFirstPersonCharacter::SetupPlayerInputComponent() - Failed to create input"));
	}
}

void AOxiFirstPersonCharacter::OnStartFire(const FInputActionValue& Value)
{
	StartFireWeapon(FirstPersonCameraComponent);
	IsFireDown = true;
}

void AOxiFirstPersonCharacter::OnStopFire(const FInputActionValue& Value)
{
	IsFireDown = false;
}

void AOxiFirstPersonCharacter::OnInteraction(const FInputActionValue& Value)
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;

	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * 300);

	TArray<FHitResult> Hits;
	float Radius = 30.0f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape); //Don't forget to comment out the line trace
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	for (FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())//Include interface
			{
				APawn* MyPawn = Cast<APawn>(MyOwner); //Cast as Pawn because Execute_Interact needs a Pawn and not an Actor
				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
				break;//Break out of for loop instead of returning
			}
		}
		//Debugging
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColor, false, 2.0f);
		GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);
	}
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

void AOxiFirstPersonCharacter::OnDeath(class UOxiDamageComponent* const DamageComp, AActor* const Victim, AActor* const Killer)
{
	//Super::OnDeath(DamageComp, Victim, Killer);
	Super::OnDeath_Internal(DamageComp, Victim, Killer);
	GetCharacterMovement()->SetActive(false);
	GetController()->SetIgnoreMoveInput(true);
	GetController()->SetIgnoreLookInput(true);

	OnDeath_Internal(Cast<UOxiHumanDamageComponent>(DamageComp), Victim, Killer);

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

void AOxiFirstPersonCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AOxiFirstPersonCharacter::Look(const struct FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

AOxiPlayerController::AOxiPlayerController()
{
	CheatClass = UOxiCheatManager::StaticClass();
}
