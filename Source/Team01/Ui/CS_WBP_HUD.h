#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WBP_KillConfirm.h"
#include "CS_WBP_KillLogEntry.h"
#include "CS_WBP_HUD.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
class UVerticalBox;

UCLASS()
class TEAM01_API UCS_WBP_HUD : public UUserWidget
{
	GENERATED_BODY()
public:
	// 탄약 텍스트를 갱신하는 함수
	void UpdateAmmoText(int32 CurrentAmmo, int32 MaxAmmo);
	// 체력바 퍼센트를 갱신하는 함수
	void UpdateHP(float NewPercent);
	// 킬 수를 갱신하는 함수
	void UpdateKillCount(int32 Kills);
	// 점수 텍스트를 갱신하는 함수
	void UpdateScore(int32 ScoreValue);
	// 미션 상태 텍스트를 갱신하는 함수
	void UpdateMissionStatus(const FString& StatusText);
	// KillConfirm 메시지를 표시하는 함수
	void ShowKillConfirmMessage(const FString& Message);

	void ShowHitMarker();

	void AddKillLogEntry(const FString& KillerName, const FString& VictimName, UTexture2D* Icon);
	
protected:
	virtual void NativeConstruct() override;

	FTimerHandle HitMarkerTimer;
	void RemoveKillLogEntry(UUserWidget* EntryToRemove);
	// 탄약 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Ammo;
	// 체력바
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;
	// 킬 수 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCount;
	// 점수 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Score;
	// 미션 상태 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MissionStatusText;
	// 조준선 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> Crosshair;
	// 히트마커 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> HitMarker;
	// 킬 로그 패널
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> KillLogPanel;

	UPROPERTY(EditDefaultsOnly,  Category="UI")
	TSubclassOf<UCS_WBP_KillLogEntry> KillLogEntryClass;
	// 적 처치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCS_WBP_KillConfirm> KillConfirm;
};

