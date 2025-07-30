#include "CS_WBP_FloatingDamageText.h"
#include "Components/TextBlock.h"

// 위젯이 생성되면 호출됩니다
void UCS_WBP_FloatingDamageText::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 데미지 텍스트를 "0"으로 설정
	if (Damage)
	{
		Damage->SetText(FText::FromString(TEXT("0")));
	}
}

// 데미지를 정수 값으로 받아 텍스트로 출력합니다
void UCS_WBP_FloatingDamageText::SetDamage(int32 DamageValue)
{
	if (Damage)
	{
		Damage->SetText(FText::AsNumber(DamageValue));
	}
}