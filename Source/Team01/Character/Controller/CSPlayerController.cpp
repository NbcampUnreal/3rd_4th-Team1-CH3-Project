#include "CSPlayerController.h"
#include "../Source/Team01/Ui/CS_WBP_HUD.h"
#include "../Ui/CSGameClear.h"
#include "Blueprint/UserWidget.h"
#include "../Game/CSGameStateBase.h"
#include "CSPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);

	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}
	
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
	}

	FString CurrentLevelName = GetWorld()->GetMapName();
	
	if (CurrentLevelName.Contains(TEXT("MenuLevel")))
	{
		ShowMainMenu(false);
	}
	else
	{
		CreateHUD();
	}
}

void ACSPlayerController::CreateHUD()
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	HUDWidget = CreateWidget<UCS_WBP_HUD>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();

		if (ACSPlayerCharacter* MyChar = Cast<ACSPlayerCharacter>(GetPawn()))
		{
			HUDWidget->UpdateAmmoText(MyChar->GetBulletCount(), MyChar->GetMaxBulletCount());
			MyChar->OnBulletChanged.AddDynamic(this, &ACSPlayerController::OnBulletChanged_Handler);
		}
		
		if (ACSGameStateBase* GS = GetWorld()->GetGameState<ACSGameStateBase>())
		{
			const EMissionState State = GS->GetCurrentMissionState();
			const int32 CurrentKillCount = GS->GetTotalKillCount();

			const FString MissionText = ACSGameStateBase::GetMissionText(State, CurrentKillCount);
			HUDWidget->UpdateMissionStatus(MissionText);

			HUDWidget->UpdateScore(GS->GetScore());
		}
		
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void ACSPlayerController::UpdateAmmoHUD(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (HUDWidget)
	{
		HUDWidget->UpdateAmmoText(CurrentAmmo, MaxAmmo);
	}
}

void ACSPlayerController::OnBulletChanged_Handler(int32 NewBulletCount)
{
	if (HUDWidget && IsValid(HUDWidget))
	{
		if (ACSPlayerCharacter* MyChar = Cast<ACSPlayerCharacter>(GetPawn()))
		{
			HUDWidget->UpdateAmmoText(NewBulletCount, MyChar->GetMaxBulletCount());
		}
	}
}

void ACSPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD 제거
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	// 기존 메뉴 제거
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
	}

	// 메뉴 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("MainMenuWidget 생성 완료"));

			bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			// 텍스트 버튼 변경
			if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidget->GetWidgetFromName(TEXT("StartTEXT"))))
			{
				ButtonText->SetText(FText::FromString(bIsRestart ? TEXT("Restart") : TEXT("Start")));
			}

			if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidget->GetWidgetFromName(TEXT("StartTEXT"))))
			{
				ButtonText->SetText(FText::FromString(bIsRestart ? TEXT("Restart") : TEXT("Start")));
			}

			if (UTextBlock* TitleText = Cast<UTextBlock>(MainMenuWidget->GetWidgetFromName(TEXT("Title"))))
			{
				TitleText->SetText(FText::FromString(bIsRestart ? TEXT("Game Over!!") : TEXT("Constantine")));
				TitleText->SetVisibility(ESlateVisibility::Visible);

				if (bIsRestart)
				{
					GetWorldTimerManager().SetTimer(
						GameOverTitleBlinkTimer,
						this,
						&ACSPlayerController::ToggleGameOverTitleVisibility,
						0.5f,
						true		
					);
				}
				else
				{
					GetWorldTimerManager().ClearTimer(GameOverTitleBlinkTimer);
				}
			}
		}
	}
}

void ACSPlayerController::ShowOptionMenu()
{
	if (OptionWidgetClass)
	{
		if (MainMenuWidget)
		{
			MainMenuWidget->RemoveFromParent();
			MainMenuWidget = nullptr;
		}
		
		if (OptionWidget)
		{
			OptionWidget->RemoveFromParent();
			OptionWidget = nullptr;
		}

		OptionWidget = CreateWidget<UUserWidget>(this, OptionWidgetClass);
		if (OptionWidget)
		{
			OptionWidget->AddToViewport();

			// 입력 모드 UIOnly로 전환 (필요 시)
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(OptionWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;

			UE_LOG(LogTemp, Warning, TEXT("OptionMenu 생성 완료"));
		}
	}
}

void ACSPlayerController::ToggleGameOverTitleVisibility()
{
	if (MainMenuWidget)
	{
		if (UTextBlock* TitleText = Cast<UTextBlock>(MainMenuWidget->GetWidgetFromName(TEXT("Title"))))
		{
			ESlateVisibility CurrentVisibility = TitleText->GetVisibility();
			TitleText->SetVisibility(CurrentVisibility == ESlateVisibility::Visible
				? ESlateVisibility::Hidden
				: ESlateVisibility::Visible);
		}
	}
}

void ACSPlayerController::StartGame()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
	}

	// 입력 모드 GameOnly로 설정
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// 마우스 커서 숨기기
	bShowMouseCursor = false;

	UGameplayStatics::OpenLevel(this, FName("MainLevel")); // ← 실제 맵 이름으로 교체
}

void ACSPlayerController::AddKillCount()
{
	++KillCount;
	
	if (HUDWidget)
	{
		HUDWidget->UpdateKillCount(KillCount);
	}

	if (ACSGameStateBase* GS = GetWorld()->GetGameState<ACSGameStateBase>())
	{
		GS->AddKill();

		GS->AddScore(100);
	}
	
	// 포탈 생성 조건 검사
	if (!bPortalSpawned && KillCount >= 9 && PortalToSpawn)
	{
		GetWorld()->SpawnActor<AActor>(PortalToSpawn, PortalSpawnLocation, FRotator::ZeroRotator);
		bPortalSpawned = true;

		UE_LOG(LogTemp, Warning, TEXT("Portal creation!"));

		if (SpawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, PortalSpawnLocation);
		}
	}

}

void ACSPlayerController::AddScore(int32 ScoreValue)
{
	Score += ScoreValue;

	if (HUDWidget)
	{
		HUDWidget->UpdateScore(Score);
	}
}

void ACSPlayerController::ShowGameClearUI()
{
	UE_LOG(LogTemp, Warning, TEXT("[PlayerController] ShowGameClearUI 호출됨"));
	UE_LOG(LogTemp, Warning, TEXT("[PlayerController] GameClearUIWidget: %s"), *GetNameSafe(GameClearUIWidget));
	UE_LOG(LogTemp, Warning, TEXT("[PlayerController] GameClearUIWidgetClass: %s"), *GetNameSafe(GameClearUIWidgetClass));

	if (!GameClearUIWidget && GameClearUIWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayerController] GameClearUI 위젯 생성 시작"));
		GameClearUIWidget = CreateWidget<UUserWidget>(this, GameClearUIWidgetClass);
	}

	if (GameClearUIWidget && !GameClearUIWidget->IsInViewport())
	{
		GameClearUIWidget->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("[PlayerController] GameClearUI 화면에 표시됨"));

		bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameClearUIWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		if (UCS_WBP_GameClearUI* ClearUI = Cast<UCS_WBP_GameClearUI>(GameClearUIWidget))
		{
			ClearUI->SetFinalScore(Score); // 또는 GetScore()
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerController] GameClearUIWidget 생성 실패 또는 이미 표시 중"));
	}
}