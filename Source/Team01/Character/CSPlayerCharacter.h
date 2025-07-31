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

// 잔탄 확인을 위한 Delegate 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletChanged, int32, NewBulletCount);

UCLASS()
class TEAM01_API ACSPlayerCharacter : public ACSCharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter
	
public:
	ACSPlayerCharacter();
	
	void RecoverHealth(float Amount);

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	
	void StartSprint(const FInputActionValue& InValue);
	void StopSprint(const FInputActionValue& InValue);
	void StartCrouch(const FInputActionValue& InValue);
	void StopCrouch(const FInputActionValue& InValue);

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsCrouching() const { return bIsCrouching; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCSInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

#pragma endregion

#pragma region Attack

public:
	UFUNCTION(BlueprintCallable)
	int32 GetBulletCount() const { return Bullet; }
	UFUNCTION(BlueprintCallable)
	int32 GetMaxBulletCount() const { return MaxBullet; }

	float GetMaxShootRange() const { return MaxShootRange; }

	UFUNCTION(BlueprintCallable)
	void Reload();

	// 추후 CharacterBase로 옮길 수 있음
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	// 위에서 선언한 Delegate 변수
	UPROPERTY(BlueprintAssignable)
	FOnBulletChanged OnBulletChanged;

private:
	void InputShoot(const FInputActionValue& InValue);
	void InputReload(const FInputActionValue& InValue);

	bool ConsumeBullet(); // 총알 소비 로직
	void TryFire(); // 실제 사격 로직

protected:
	int32 Bullet;
	int32 MaxBullet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxShootRange = 20000.f;

	
#pragma endregion

#pragma region Animation

public:
	// 사격 애니메이션만 담당
	virtual void BeginAttack() override; 
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ShootMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;

	// EndAttack 으로 전달하기 위한 delegate
	FOnMontageEnded OnShootMontageEndedDelegate;
	FOnMontageEnded OnReloadMontageEndedDelegate;
	
#pragma endregion

};
