#include "CS_WBP_KillLogEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

// 위젯이 생성될 때 호출됩니다
void UCS_WBP_KillLogEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

// 킬 로그 데이터를 세팅
// Killer: 킬한 사람 이름, Victim: 죽은 사람 이름, WeaponIcon: 사용된 무기 아이콘
void UCS_WBP_KillLogEntry::SetupKillLog(const FString& Killer, const FString& Victim, UTexture2D* WeaponIcon)
{
	// 킬러 이름 설정
	if (KillerNameText)
	{
		KillerNameText->SetText(FText::FromString(Killer));
	}

	// 피해자 이름 설정
	if (VictimNameText)
	{
		VictimNameText->SetText(FText::FromString(Victim));
	}

	// 무기 아이콘 이미지 설정
	if (WeaponIconImage && WeaponIcon)
	{
		WeaponIconImage->SetBrushFromTexture(WeaponIcon);
	}
}
