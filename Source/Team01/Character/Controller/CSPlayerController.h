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

#pragma region HUD
public:
	FORCEINLINE UCS_WBP_HUD* GetHUDWidget() const { return HUDWidget; } // HUD 접근 가능하도록 Getter 추가

private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCS_WBP_HUD> HUDWidgetClass;  // 위젯 블루프린트 지정용

	UPROPERTY()
	TObjectPtr<UCS_WBP_HUD> HUDWidget;        // 위젯 인스턴스 저장용

	// HUD 생성 함수
	void CreateHUD();
#pragma endregion HUD
};
