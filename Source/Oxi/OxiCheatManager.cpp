// ELP 2023

#include "OxiCheatManager.h"

#include "Engine/PawnIterator.h"
#include "Kismet/GameplayStatics.h"

#include "OxiCharacter.h"
#include "OxiCheckPoint.h"

void UOxiCheatManager::WarpToCheckPoint(FString CheckpointName)
{
	for (TActorIterator<AOxiCheckpoint> It(GetWorld()); It; ++It)
	{
		if (It->GetActorNameOrLabel() == CheckpointName)
		{
			ACharacter* const PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (PlayerChar == nullptr)
			{
				return;
			}
			It->ReloadCheckpoint(PlayerChar);
			break;
		}
	}
}
