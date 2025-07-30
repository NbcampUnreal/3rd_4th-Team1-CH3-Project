#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CS_WBP_KillLogEntry.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class TEAM01_API UCS_WBP_KillLogEntry : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 킬 로그 정보를 설정하는 함수
	void SetupKillLog(const FString& Killer, const FString& Victim, UTexture2D* WeaponIcon);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillerNameText; // 킬러 이름

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> WeaponIconImage; // 무기 아이콘

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> VictimNameText; // 피해자 이름
};
