#include "../Ui/CS_WBP_EnemyHPBar.h"
#include "Components/ProgressBar.h"

void UCS_WBP_EnemyHPBar::SetHPBarPercent(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercent(Percent);
	}
}