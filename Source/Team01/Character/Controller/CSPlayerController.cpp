#include "CSPlayerController.h"
#include "../Source/Team01/Ui/CS_WBP_HUD.h"
#include "Blueprint/UserWidget.h"
#include "CSPlayerCharacter.h"

ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);

	CreateHUD();
}

void ACSPlayerController::CreateHUD()
{
	// HUD 위젯 생성
	HUDWidget = CreateWidget<UCS_WBP_HUD>(this, HUDWidgetClass);

	// 유효하면 화면에 표시
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();

		
		if (ACSPlayerCharacter* MyChar = Cast<ACSPlayerCharacter>(GetPawn()))
		{
			// 현재 탄약 표시 초기화
			HUDWidget->UpdateAmmoText(MyChar->GetBulletCount(), MyChar->GetMaxBulletCount());

			// 탄약 변경 델리게이트 바인딩
			MyChar->OnBulletChanged.AddDynamic(this, &ACSPlayerController::OnBulletChanged_Handler);
		}
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