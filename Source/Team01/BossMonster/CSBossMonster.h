#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "CSBossMonster.generated.h"

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
    Melee,      // 일반 근접 공격
    GroundSlam  // 새로 추가할 점프 후 바닥 찍기 공격
};

UCLASS()
class TEAM01_API ACSBossMonster : public ACSCharacterBase
{
	GENERATED_BODY()
	
public:
    ACSBossMonster();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> HitReactMontage; //맞았을 때 히트 애니메이션 몽타주

    void BeginAttackPattern(EBossAttackType AttackType);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TMap<EBossAttackType, TObjectPtr<UAnimMontage>> AttackMontages;

    virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted) override;

    void AttackHitCheck(); // Melee 공격의 데미지를 실제로 적용하는 함수

    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyGroundSlamDamage();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void Die(); //죽었을 때 처리될 함수

    void GoRagdoll(); //죽고나서 자연스럽게 보이기위한 함수

    void Disappear(); //소멸함수

    void EnterPhase2(); //페이즈 2 진입 함수

    void TryStateTransition(); // 상태 전환을 시도하는 함수 선언

    bool GetIsInPhase2() const { return bIsInPhase2; } //Phase 2 Bool값을 전해주는 Get함수

    UPROPERTY(EditInstanceOnly, Category = "AI")
    TArray<TObjectPtr<AActor>> PatrolPoints; //패트롤 위치 타겟 포인트를 저장하는 Array

protected:

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamDamage = 50.0f; // 공격력

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamRadius = 500.0f; // 공격 반경 (5미터)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamForwardOffset = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    TObjectPtr<UParticleSystem> GroundSlamVFX; // 바닥 찍을 때 터질 파티클 효과

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    TObjectPtr<USoundBase> GroundSlamSFX; // 바닥 찍을 때 재생될 사운드

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State") // 2 페이즈 여부를 저장하는 Bool 변수
    bool bIsInPhase2;

    bool bIsPhaseTransitionPending; // 2페이즈

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // 2 페이즈 진입 애니메이션
    TObjectPtr<UAnimMontage> Phase2TransitionMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX") // 2 페이즈 진입 파티클
    TObjectPtr<UParticleSystem> Phase2TransitionVFX;

    FTimerHandle DeathTimerHandle;

    FTimerHandle DisappearTimerHandle;
    
    FTimerHandle HPHideTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HPBarComponent;

	UPROPERTY()
	TObjectPtr<class UCS_WBP_EnemyHPBar> HPBar;

	UPROPERTY()
	AController* LastInstigator = nullptr;
};
