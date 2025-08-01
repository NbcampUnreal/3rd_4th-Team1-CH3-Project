#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "CSBossMonster.generated.h"


UCLASS()
class TEAM01_API ACSBossMonster : public ACSCharacterBase
{
	GENERATED_BODY()
	
public:
    ACSBossMonster();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> AttackMontage; // 공격 할 때 애니메이션 몽타주

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> HitReactMontage; //맞았을 때 히트 애니메이션 몽타주

    virtual void BeginAttack() override;

    virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

    void AttackHitCheck();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void Die(); //죽었을 때 처리될 함수

    void GoRagdoll(); //죽고나서 자연스럽게 보이기위한 함수

    void Disappear(); //소멸함수

    UPROPERTY(EditInstanceOnly, Category = "AI")
    TArray<TObjectPtr<AActor>> PatrolPoints;

protected:

    virtual void BeginPlay() override;

    FTimerHandle DeathTimerHandle;

    FTimerHandle DisappearTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HPBarComponent;

	UPROPERTY()
	TObjectPtr<class UCS_WBP_EnemyHPBar> HPBar;
};
