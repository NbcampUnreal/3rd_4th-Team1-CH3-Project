#include "../Game/CSGameStateBase.h"
#include "CSGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/Controller/CSPlayerController.h"
#include "../Ui/CS_WBP_HUD.h"   

void ACSGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetMissionState(EMissionState::KillEnemies);
}

FString ACSGameStateBase::GetMissionText(EMissionState State, int32 KillCount)
{
	switch (State)
	{
	case EMissionState::KillEnemies:
		return FString::Printf(TEXT("적을 처치하십시오 (%d / 15)"), KillCount);
	case EMissionState::BossFight:
		return TEXT("보스를 처치하십시오");
	case EMissionState::MissionClear:
		return TEXT("미션 완료!");
	default:
		return TEXT("");
	}
}

void ACSGameStateBase::AddKill()
{
	TotalKillCount++;

	if (TotalKillCount == 1)
	{
		SetMissionState(EMissionState::KillEnemies);
	}
	else if (TotalKillCount == 14)
	{
		SetMissionState(EMissionState::BossFight);
	}
	else if (CurrentMissionState == EMissionState::KillEnemies)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (ACSPlayerController* CSController = Cast<ACSPlayerController>(PC))
			{
				if (UCS_WBP_HUD* HUD = CSController->GetHUDWidget())
				{
					HUD->UpdateMissionStatus(GetMissionText(CurrentMissionState, TotalKillCount));
				}
			}
		}
	}
}

void ACSGameStateBase::AddScore(int32 Amount)
{
	TotalScore += Amount;
	
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ACSPlayerController* CSController = Cast<ACSPlayerController>(PC))
		{
			if (UCS_WBP_HUD* HUD = CSController->GetHUDWidget())
			{
				HUD->UpdateScore(TotalScore);
			}
		}
	}
}

void ACSGameStateBase::SetMissionState(EMissionState NewState)
{
	CurrentMissionState = NewState;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ACSPlayerController* CSController = Cast<ACSPlayerController>(PC))
		{
			if (UCS_WBP_HUD* HUD = CSController->GetHUDWidget())
			{
				FString MissionText = GetMissionText(NewState, TotalKillCount);
				if (!MissionText.IsEmpty())
				{
					HUD->UpdateMissionStatus(MissionText);
				}
			}
		}
	}
}