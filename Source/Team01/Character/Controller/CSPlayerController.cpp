#include "CSPlayerController.h"
#include "../Source/Team01/Ui/CS_WBP_HUD.h"
#include "Blueprint/UserWidget.h"
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
}