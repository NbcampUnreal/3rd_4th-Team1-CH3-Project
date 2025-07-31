#include "CSBossMonster.h"
#include "CSBossAIController.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "GameFramework/CharacterMovementComponent.h"


ACSBossMonster::ACSBossMonster()
{
	MaxHP = 5000.0f;
	CurrentHP = MaxHP;
	AttackDamage = 100.0f;

	AIControllerClass = ACSBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(200.f, 25.f)); // 보스는 약간 더 큼직하게
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
}

void ACSBossMonster::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
}


void ACSBossMonster::BeginAttack() //공격 시작 로직
{
	// 변경: bIsNowAttacking = true; 대신 상태를 'Attacking'으로 설정합니다.
	SetCurrentState(ECharacterState::Attacking);
	UE_LOG(LogTemp, Log, TEXT("Boss has started its attack!"));
}


void ACSBossMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped) //공격 종료 로직
{
	// 변경: bIsNowAttacking = false; 대신 상태를 'Idle'로 설정합니다.
	SetCurrentState(ECharacterState::Idle);


	UE_LOG(LogTemp, Log, TEXT("Boss has finished its attack. Interrupted: %s"), bInterruped ? TEXT("Yes") : TEXT("No"));
}

//데미지를 받았을 때 호출될 함수
float ACSBossMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 변경: !bIsDead 대신 GetIsDead() 함수 또는 상태 직접 비교로 변경합니다.
	if (FinalDamage > 0.f && GetCurrentState() != ECharacterState::Dead)
	{
		CurrentHP -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Boss took %f damage, Current Health: %f"), FinalDamage, CurrentHP);

		if (CurrentHP <= 0.f)
		{
			Die();
		}
		else
		{
			// TODO: 피격 애니메이션
		}
	}

	return FinalDamage;
}

//사망 처리 함수
void ACSBossMonster::Die()
{
	// 변경: bIsDead를 직접 확인하는 대신 GetIsDead() 또는 상태를 직접 확인합니다.
	if (GetCurrentState() == ECharacterState::Dead) return;

	// 변경: bIsDead = true; 대신 상태를 'Dead'로 설정합니다.
	SetCurrentState(ECharacterState::Dead);

	UE_LOG(LogTemp, Error, TEXT("Boss is Dead!"));
}