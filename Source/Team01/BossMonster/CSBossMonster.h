#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "CSBossMonster.generated.h"

USTRUCT(BlueprintType)
struct FAttackMontageArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<UAnimMontage>> Montages;
};

UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
    Melee,      // 일반 근접 공격
    GroundSlam,  // 새로 추가할 점프 후 바닥 찍기 공격
    RollAndCharge
};

UCLASS()
class TEAM01_API ACSBossMonster : public ACSCharacterBase
{
	GENERATED_BODY()
	
public:
    ACSBossMonster();

    void BeginAttackPattern(EBossAttackType AttackType);

    virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted) override;

    void BeginChargeAttack();

    void EndChargeAttack();

    void BeginStun();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void Die(); //죽었을 때 처리될 함수

    void GoRagdoll(); //죽고나서 자연스럽게 보이기위한 함수

    void Disappear(); //소멸함수

    void EnterPhase2(); //페이즈 2 진입 함수

    void TryStateTransition(); // 상태 전환을 시도하는 함수 선언

    bool GetIsInPhase2() const { return bIsInPhase2; } //Phase 2 Bool값을 전해주는 Get함수

    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyMeleeDamage(); // Melee 공격의 데미지를 실제로 적용하는 함수

    UFUNCTION(BlueprintCallable, Category = "Attack")
    void ApplyGroundSlamDamage(); // GroundSlam 공격의 데미지를 실제로 적용하는 함수

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> HitReactMontage; //맞았을 때 히트 애니메이션 몽타주

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TMap<EBossAttackType, FAttackMontageArray> AttackMontages; //공격 패턴 몽타주 모음

    UPROPERTY(EditInstanceOnly, Category = "AI")
    TArray<TObjectPtr<AActor>> PatrolPoints; //패트롤 위치 타겟 포인트를 저장하는 Array

#pragma region Attack Attributes
    // ===== Ground Slam Attack 관련 속성 =====

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamDamage = 50.0f; // 공격력

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamRadius = 500.0f; // 공격 반경 (5미터)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamForwardOffset = 150.0f; // 공격 판정 위치

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamJumpXYPower = 1000.0f; // GroundSlam 시 점프하는 힘 (수평 방향)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamJumpZPower = 500.0f; // GroundSlam 시 점프하는 힘 (수직 방향, 높이)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    float GroundSlamGravityScale = 3.0f; // GroundSlam 시 적용될 중력 배율

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    TObjectPtr<UParticleSystem> GroundSlamVFX; // 바닥 찍을 때 터질 파티클 효과

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GroundSlam")
    TObjectPtr<USoundBase> GroundSlamSFX; // 바닥 찍을 때 재생될 사운드
#pragma endregion

protected:

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    float OriginalWalkSpeed; //원래 이동속도

    float DefaultGravityScale;

    // 스턴 애니메이션 몽타주와 벽 충돌 사운드를 에디터에서 설정할 수 있도록 변수 추가
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> StunMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    TObjectPtr<USoundBase> WallHitSound;

#pragma region Dynamic Materials

    UPROPERTY()
    TArray<TObjectPtr<UMaterialInstanceDynamic>> BossDynamicMaterials;

    // 머티리얼 전환 효과를 제어할 변수들
    bool bIsTransitioningMaterial = false;

    // GlowAmount 파라미터 제어용 변수
    float CurrentGlowAmount = 32.0f; 
    float TargetGlowAmount = 32.0f;

    // Enrage 파라미터 제어용 변수
    float CurrentEnrageValue = 0.3f; 
    float TargetEnrageValue = 0.3f;

    float MaterialTransitionSpeed = 0.5f; // 전환 속도

#pragma endregion

#pragma region Phase 2 Attributes
    // ===== 2 Phase 관련 속성 =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State") // 2 페이즈 여부를 저장하는 Bool 변수
    bool bIsInPhase2;

    bool bIsPhaseTransitionPending; // 2페이즈

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation") // 2 페이즈 진입 애니메이션
    TObjectPtr<UAnimMontage> Phase2TransitionMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX") // 2 페이즈 진입 파티클
    TObjectPtr<UParticleSystem> Phase2TransitionVFX;
#pragma endregion


#pragma region Death, Disappear Attributes
    // ===== Death, Disappear 관련 속성 =====

    FTimerHandle DeathTimerHandle;

    FTimerHandle DisappearTimerHandle;
#pragma endregion
    

#pragma region UI Attributes
    // ===== UI 관련 속성 ===== 
    FTimerHandle HPHideTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> HPBarComponent;

	UPROPERTY()
	TObjectPtr<class UCS_WBP_EnemyHPBar> HPBar;

	UPROPERTY()
	AController* LastInstigator = nullptr;
#pragma endregion


private:

    FVector ChargeDirection;         // 현재 돌격하고 있는 방향
    float ChargeSpeed = 5000.0f;     // 돌격 속도
    TArray<AActor*> DamagedActorsInCharge;

    FTimerHandle StunEndTimerHandle; // 스턴 상태가 끝나는 것을 관리할 타이머 핸들
};
