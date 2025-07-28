#include "CSBaseStatComponent.h"

UCSBaseStatComponent::UCSBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false; //안씀
	bWantsInitializeComponent = true;

	MaxHp = 100.0f; //최대 체력 초기화
}


void UCSBaseStatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCSBaseStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CurrentHp = MaxHp; //비긴 플레이가 아닌 이 타이밍에 값 설정해놓기
}

void UCSBaseStatComponent::TakeDamage(float Damage)
{
	CurrentHp = FMath::Clamp(CurrentHp - Damage, 0.0f, MaxHp);
	UE_LOG(LogTemp, Warning, TEXT("BaseStatComponent::TakeDamage >> CurrentHP: %f"), CurrentHp); //대충 지금은 로그 찍어놓기

	if (FMath::IsNearlyZero(CurrentHp))
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is Dead.")); // 지금은 대충 로그 찍어놨지만 나중에 기능 구현
	}
}


