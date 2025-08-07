#pragma once

#include "CoreMinimal.h"
#include "CSCharacterBase.h"
#include "InputActionValue.h"
#include "CSPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCSInputConfig;
class UInputMappingContext;
class UAnimMontage;
class UParticleSystem;
class UParticleSystemComponent;

UENUM(BlueprintType)
enum class EUltCastState : uint8
{
	None,
	Casting,
	ReadyToFire,
	CoolDown
};

// 잔탄 확인을 위한 Delegate 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletChanged, int32, NewBulletCount);

// IK Goal 데이터를 위한 구조체 (위치, 회전)
USTRUCT(BlueprintType)
struct FIKGoalData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FTransform GoalTransform; // IK Goal의 Transform (위치, 회전, 스케일)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float Alpha; // IK Goal의 적용 강도 (0.0 = 미적용, 1.0 = 완전 적용)

	FIKGoalData()
	: GoalTransform(FTransform::Identity)
	, Alpha(1.0f)
	{}
};

UCLASS()
class TEAM01_API ACSPlayerCharacter : public ACSCharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter
	
public:
	ACSPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

	
#pragma endregion

#pragma region Character Status

public:
	void RecoverHealth(float Amount);

	
#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	
	void StartSprint(const FInputActionValue& InValue);
	void StopSprint(const FInputActionValue& InValue);
	void StartCrouch(const FInputActionValue& InValue);
	void StopCrouch(const FInputActionValue& InValue);

	void StartIronSight(const FInputActionValue& InValue);
	void StopIronSight(const FInputActionValue& InValue);

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsCrouching() const { return bIsCrouching; }
	UFUNCTION()
	void TryActivateNearbyItem();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCSInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

	float TargetFOV = 70.f;
	float CurrentFOV = 70.f;

#pragma endregion

#pragma region Attack

public:
	UFUNCTION(BlueprintCallable)
	int32 GetBulletCount() const { return Bullet; }
	UFUNCTION(BlueprintCallable)
	int32 GetMaxBulletCount() const { return MaxBullet; }

	float GetMaxShootRange() const { return MaxShootRange; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* HitParticleSystem;

	UFUNCTION(BlueprintCallable)
	void Reload();

	void TryFire(); // 실제 사격 로직

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// 디버그 시각화를 위한 헬퍼 함수
	void DrawDebugVisualization(
		const FVector& WeaponLocation,
		const FVector& CameraLocation,
		const FVector& TargetLocation,
		const FHitResult& HitResult,
		bool bHit,
		const FVector& EndLocation,
		int32 DebugType);

	// 히트 처리를 위한 헬퍼 함수
	void ProcessHit(const FHitResult& HitResult);

	
public:
	// 위에서 선언한 Delegate 변수
	UPROPERTY(BlueprintAssignable)
	FOnBulletChanged OnBulletChanged;

private:
	void InputShoot(const FInputActionValue& InValue);
	void InputReload(const FInputActionValue& InValue);

	bool ConsumeBullet(); // 총알 소비 로직

protected:
	int32 Bullet;
	int32 MaxBullet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxShootRange = 4000.f;

	
#pragma endregion

#pragma region Animation

public:
	// 사격 애니메이션만 담당
	virtual void BeginAttack() override; 
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted) override;

	// 피격 시 레그돌 기능
	UFUNCTION()
	void OnHittedRagdollRestoreTimerElapsed();

	// 사망 애니메이션 재생
	UFUNCTION()
	void IsDying();

	// IK Rig 노드에서 사용할 Left Hand IK Goal 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
	FIKGoalData LeftHandIKGoal;

protected:
	UFUNCTION()
	FTransform CalculateLeftHandIKGoalTransform();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> OnHitMontage;

	// EndAttack 으로 전달하기 위한 delegate
	FOnMontageEnded OnShootMontageEndedDelegate;
	FOnMontageEnded OnReloadMontageEndedDelegate;

	FTimerHandle HittedRagdollRestoreTimer;
	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	float TargetRagdollBlendWeight = 0.f;
	float CurrentRagdollBlendWeight = 0.f;
	bool bIsNowRagdollBlending = false;
	
#pragma endregion

#pragma region Ult

protected:
	void OnUltCastPressed(const FInputActionValue& InValue);
	void OnUltCastReleased(const FInputActionValue& InValue);

	UFUNCTION(BlueprintCallable)
	void FireUltEffect();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUltCastState UltCastState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MinCastTime = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxCastTime = 3.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentCastTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CoolDownTime = 5.f;

	FTimerHandle CastTimerHandle;
	FTimerDelegate TimerDelegateForMaxCast;
	FTimerHandle CastUpdateTimerHandle;
	FTimerDelegate TimerDelegateForCastUpdate;
	FTimerHandle CoolDownTimerHandle;
	FTimerDelegate TimerDelegateForCoolDown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* UltCastParticleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* UltFireParticleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundCue* UltCastSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	USoundWave* UltFireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<AActor> UltProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UltCastStartSectionName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UltCastLoopSectionName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UltCastEndSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> UltCastingMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> UltFireMontage;

private:
	UPROPERTY()
	UParticleSystemComponent* UltCastParticleEffectComponent;

	void StartCastingEffect();
	void StopCastingEffect();
	
#pragma endregion

};
