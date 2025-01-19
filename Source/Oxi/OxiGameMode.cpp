// ELP 2020

#include "OxiGameMode.h"
#include "OxiHUD.h"
#include "OxiCharacter.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogOxiCombat);

UCombatManager* UCombatManager::Instance = nullptr;

UCombatManager::UCombatManager()
{
	if (HasAnyFlags(RF_ArchetypeObject) == false)
	{
		Instance = this;
	}
}

void UCombatManager::BeginDestroy()
{
	Super::BeginDestroy();

	if (Instance == this)
	{
		Instance = nullptr;
	}
}

void UCombatManager::TriggerDeathEvent(class UOxiDamageComponent* Victim, UOxiDamageComponent* Killer)
{
	if (Instance == nullptr)
	{
		UE_LOG(LogOxiCombat, Log, TEXT("UCombatManager::UnregisterEventListener() - NULL combat manager."));
		return;
	}

	Instance->CharacterDeathDelegates.Broadcast(Victim, Killer);
}

AOxiGameMode::AOxiGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
//	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AOxiHUD::StaticClass();
	this->PlayerControllerClass = AOxiPlayerController::StaticClass();
	//if (GIsPlayInEditorWorld || GIsEditor == false)
	{
		CombatManager = CreateDefaultSubobject<UCombatManager>(TEXT("CombatManager"));

		//check(GCombatManager == nullptr);

	}
}

void AOxiGameMode::BeginDestroy()
{
	Super::BeginDestroy();

//s	if (GIsPlayInEditorWorld || GIsEditor == false)
	{
	//	check(GCombatManager != nullptr);

	}
}