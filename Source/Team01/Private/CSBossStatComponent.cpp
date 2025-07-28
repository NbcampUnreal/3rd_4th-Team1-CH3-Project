#include "CSBossStatComponent.h"

void UCSBossStatComponent::TakeDamage(float Damage)
{
	
	Super::TakeDamage(Damage); // 부모 클래스의 TakeDamage 그대로 실행

	// 여기에 보스 전용 로직
	// 실험이긴한데 데미지를 입을 때마다 스페셜 게이지가 오르는 느낌?
	SpecialGauge = FMath::Clamp(SpecialGauge + Damage * 0.1f, 0.0f, 100.0f);
	UE_LOG(LogTemp, Warning, TEXT("CSBossStatComponent::TakeDamage >> SpecialGauge: %f"), SpecialGauge);
}