#include "CSGameModeBase.h"
#include "../Character/Controller/CSPlayerController.h"
#include "../Character/CSPlayerCharacter.h"

ACSGameModeBase::ACSGameModeBase()
{
	PlayerControllerClass = ACSPlayerController::StaticClass();
	DefaultPawnClass = ACSPlayerCharacter::StaticClass();
}
