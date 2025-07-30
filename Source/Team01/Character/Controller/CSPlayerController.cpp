#include "CSPlayerController.h"
#include "../Source/Team01/Ui/CS_WBP_HUD.h"
#include "Blueprint/UserWidget.h"

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
	HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);

	// 유효하면 화면에 표시
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}