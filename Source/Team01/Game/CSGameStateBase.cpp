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
		return FString::Printf(TEXT("적을 처치하십시오 (%d / 16)"), KillCount);
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
	else if (TotalKillCount == 15)
	{
		SetMissionState(EMissionState::BossFight);
	}

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
	UE_LOG(LogTemp, Warning, TEXT("[GameState] SetMissionState 호출됨: %d"), (int32)NewState);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameState] PlayerController 캐스팅 시도"));
		if (ACSPlayerController* CSController = Cast<ACSPlayerController>(PC))
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameState] PlayerController 캐스팅 성공"));

			if (NewState == EMissionState::MissionClear)
			{
				UE_LOG(LogTemp, Warning, TEXT("[GameState] MissionClear 상태 진입 - 2초 후 UI 표시 예정"));

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [CSController]()
				{
					UE_LOG(LogTemp, Warning, TEXT("[GameState] 2초 타이머 완료 → ShowGameClearUI 호출"));

					CSController->ShowGameClearUI();
				}, 2.0f, false);
			}
		}
	}
}