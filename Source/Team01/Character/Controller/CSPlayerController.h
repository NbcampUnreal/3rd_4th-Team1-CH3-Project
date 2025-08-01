#pragma once

#include "CoreMinimal.h"
#include "../Ui/CS_WBP_HUD.h"
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

	
#pragma region HUD
public:
	FORCEINLINE UCS_WBP_HUD* GetHUDWidget() const { return HUDWidget; }

	void CreateHUD();
	void UpdateAmmoHUD(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION()
	void OnBulletChanged_Handler(int32 NewBulletCount);

private:

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCS_WBP_HUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UCS_WBP_HUD> HUDWidget;
	
#pragma endregion HUD

#pragma region MainMenu

public:
	void ShowMainMenu(bool bIsRestart);
	UFUNCTION(BlueprintCallable, Category="menu")
	void StartGame();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UUserWidget> MainMenuWidget;

#pragma endregion MainMenu
};
