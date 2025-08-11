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
	void ShowGameClearUI();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameClearUIWidgetClass;
private:

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCS_WBP_HUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UCS_WBP_HUD> HUDWidget;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> MouseWidget;
	
	UUserWidget* GameClearUIWidget;
	
#pragma endregion HUD

#pragma region Menu

public:
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu(bool bIsRestart);
	
	UFUNCTION(BlueprintCallable, Category="menu")
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void ShowOptionMenu();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	TObjectPtr<UUserWidget> MainMenuWidget;

	UFUNCTION(BlueprintCallable)
	void ShowMouseMenu();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MouseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> OptionWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> OptionWidget;
	
	FTimerHandle GameOverTitleBlinkTimer;

	UFUNCTION()
	void ToggleGameOverTitleVisibility();

	// ��Ż ���� ����
	UPROPERTY(EditAnywhere, Category = "Portal")
	TSubclassOf<AActor> PortalToSpawn;

	UPROPERTY(EditAnywhere, Category = "Portal")
	FVector PortalSpawnLocation;

	bool bPortalSpawned = false;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* SpawnSound;

#pragma endregion Menu

#pragma region KillCount

protected:
	UPROPERTY()
	int32 KillCount = 0;
	UPROPERTY(VisibleAnywhere, Category = "Score")
	int32 Score = 0;

public:
	void AddKillCount();
	void AddScore(int32 ScoreValue);
	int32 GetScore() const { return Score; }
	
#pragma endregion KillCount

#pragma region mouse
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mouse")
	float MouseSensitivity = 1.0f; // 감도 기본값

	UFUNCTION(BlueprintCallable, Category="Mouse")
	void SetMouseSensitivity(float In) { MouseSensitivity = In; }

	UFUNCTION(BlueprintPure, Category="Mouse")
	float GetMouseSensitivity() const { return MouseSensitivity; }

protected:
	virtual void AddYawInput(float Val) override { Super::AddYawInput(Val * MouseSensitivity); }
	virtual void AddPitchInput(float Val) override { Super::AddPitchInput(Val * MouseSensitivity); }
#pragma endregion mouse
};
