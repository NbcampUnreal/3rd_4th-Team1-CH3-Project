#include "CS_WBP_HUD.h"
#include "CS_WBP_KillConfirm.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"

// 위젯이 처음 생성되거나 표시될 때 호출됩니다
void UCS_WBP_HUD::NativeConstruct()
{
	Super::NativeConstruct();

	// 탄약 초기화 (0 / 0)
	if (Ammo)
	{
		const FString AmmoString = FString::Printf(TEXT("%02d / %02d"), 0, 0);
		Ammo->SetText(FText::FromString(AmmoString));
	}

	// 체력바는 처음엔 최대 체력으로 시작
	if (HPBar)
	{
		HPBar->SetPercent(1.0f);
	}

	// 킬 수 초기값 설정
	if (KillCount)
	{
		KillCount->SetText(FText::FromString(TEXT("Kill: 0")));
	}

	// 점수 초기값 설정
	if (Score)
	{
		Score->SetText(FText::FromString(TEXT("Score: 0")));
	}

	// 미션 상태 텍스트 초기화
	if (MissionStatusText)
	{
		MissionStatusText->SetText(FText::FromString(TEXT("미션: 준비 중")));
	}

	if (HitMarker)
	{
		HitMarker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCS_WBP_HUD::ShowHitMarker()
{
	if (HitMarker)
	{
		HitMarker->SetVisibility(ESlateVisibility::Visible);

		// 기존 타이머 초기화 후 새로 설정
		GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimer);
		GetWorld()->GetTimerManager().SetTimer(HitMarkerTimer, [this]()
		{
			if (HitMarker)
			{
				HitMarker->SetVisibility(ESlateVisibility::Hidden);
				UE_LOG(LogTemp, Warning, TEXT("[HitMarker] Hidden"));
			}
		}, 0.2f, false);

		UE_LOG(LogTemp, Warning, TEXT("[HitMarker] Visible"));
	}
}

// 탄약 표시 텍스트를 업데이트합니다 ("12 / 30" 형식)
void UCS_WBP_HUD::UpdateAmmoText(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (Ammo)
	{
		const FString AmmoString = FString::Printf(TEXT("%02d / %02d"), CurrentAmmo, MaxAmmo);
		Ammo->SetText(FText::FromString(AmmoString));
	}
}

// 체력바의 퍼센트를 갱신합니다 (0.0 ~ 1.0 사이)
void UCS_WBP_HUD::UpdateHP(float NewPercent)
{
	if (HPBar)
	{
		HPBar->SetPercent(NewPercent);
	}
}

// 킬 수 텍스트를 "Kill: N" 형식으로 갱신합니다
void UCS_WBP_HUD::UpdateKillCount(int32 Kills)
{
	if (KillCount)
	{
		KillCount->SetText(FText::Format(
			NSLOCTEXT("HUD", "KillCount", "Kill: {0}"),
			FText::AsNumber(Kills)
		));
	}
}

// 점수 텍스트를 "Score: N" 형식으로 갱신합니다
void UCS_WBP_HUD::UpdateScore(int32 ScoreValue)
{
	if (Score)
	{
		Score->SetText(FText::Format(
			NSLOCTEXT("HUD", "Score", "Score: {0}"),
			FText::AsNumber(ScoreValue)
		));
	}
}

// 미션 상태 텍스트를 설정합니다 ("미션: 탈출하라" 등)
void UCS_WBP_HUD::UpdateMissionStatus(const FString& StatusText)
{
	if (MissionStatusText)
	{
		MissionStatusText->SetText(FText::FromString(StatusText));
	}
}

// 예: "Kill!!", "헤드샷!", "더블 킬!" 등의 문구를 전달받아 표시할 수 있습니다.
void UCS_WBP_HUD::ShowKillConfirmMessage(const FString& Message)
{
	// KillConfirm 위젯이 유효한지 확인 (nullptr 체크)
	if (KillConfirm)
	{
		// 메시지를 KillConfirm 위젯에 전달하여 화면에 출력
		KillConfirm->ShowKillMessage(Message);
	}
}
void UCS_WBP_HUD::AddKillLogEntry(const FString& Killer, const FString& Victim, UTexture2D* Icon)
{
	if (KillLogPanel && KillLogEntryClass)
	{
		UCS_WBP_KillLogEntry* NewEntry = CreateWidget<UCS_WBP_KillLogEntry>(this, KillLogEntryClass);
		if (NewEntry)
		{
			NewEntry->SetupKillLog(Killer, Victim, Icon);
			KillLogPanel->AddChildToVerticalBox(NewEntry);

			UE_LOG(LogTemp, Warning, TEXT("[KillLog] Added: %s -> %s"), *Killer, *Victim);

			// 타이머를 통해 3초 후 삭제
			FTimerHandle RemoveTimer;
			GetWorld()->GetTimerManager().SetTimer(RemoveTimer, [NewEntry]()
			{
				if (NewEntry)
				{
					NewEntry->RemoveFromParent();
					UE_LOG(LogTemp, Warning, TEXT("[KillLog] Removed"));
				}
			}, 3.0f, false);
		}
	}
}