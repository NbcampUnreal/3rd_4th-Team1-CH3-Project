#include "CSBossMonster.h"
#include "GameFramework/CharacterMovementComponent.h"

ACSBossMonster::ACSBossMonster()
{
    MaxHP = 5000.0f;
    CurrentHP = MaxHP;
    AttackDamage = 100.0f;
}

void ACSBossMonster::BeginPlay()
{
    Super::BeginPlay();

    CurrentHP = MaxHP; // 게임 시작 시 체력을 최대로 설정
}


void ACSBossMonster::BeginAttack() //공격 시작 로직
{
    bIsNowAttacking = true;
    UE_LOG(LogTemp, Log, TEXT("Boss has started its attack!"));
}


void ACSBossMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped) //공격 종료 로직
{
    bIsNowAttacking = false;
    bIsAttackKeyPressed = false; //연속 공격을 위한 키 입력 플래그 초기화
    UE_LOG(LogTemp, Log, TEXT("Boss has finished its attack. Interrupted: %s"), bInterruped ? TEXT("Yes") : TEXT("No"));
}

//데미지를 받았을 때 호출될 함수
float ACSBossMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (FinalDamage > 0.f && !bIsDead)
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
    if (bIsDead) return;

    bIsDead = true;

    UE_LOG(LogTemp, Error, TEXT("Boss is Dead!"));

}