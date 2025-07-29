#include "CSPlayerController.h"

ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}
