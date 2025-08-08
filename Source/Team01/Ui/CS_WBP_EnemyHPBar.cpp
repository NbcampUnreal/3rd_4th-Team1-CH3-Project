#include "../Ui/CS_WBP_EnemyHPBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UCS_WBP_EnemyHPBar::SetHPBarPercent(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
	}
}

void UCS_WBP_EnemyHPBar::UpdateHP(float NewPercent)
{
	SetHPBarPercent(NewPercent);
}

void UCS_WBP_EnemyHPBar::SetHP(float Current, float Max)
{
	const float SafeMax = FMath::Max(Max, 1.f);
	const float Percent = FMath::Clamp(Current / SafeMax, 0.f, 1.f);

	// 퍼센트 갱신
	SetHPBarPercent(Percent);

	// 텍스트 갱신: "80 / 100"
	if (HPText)
	{
		const int32 CurI = FMath::RoundToInt(Current);
		const int32 MaxI = FMath::RoundToInt(Max);
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurI, MaxI)));
	}
}