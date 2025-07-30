#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSPlayerController.generated.h"

class UCS_WBP_HUD;

UCLASS()
class TEAM01_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;  // 위젯 블루프린트 지정용

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;        // 위젯 인스턴스 저장용

	// HUD 생성 함수
	void CreateHUD();
};
