#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSPlayerController.generated.h"

UCLASS()
class TEAM01_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();

protected:
	virtual void BeginPlay() override;
};
