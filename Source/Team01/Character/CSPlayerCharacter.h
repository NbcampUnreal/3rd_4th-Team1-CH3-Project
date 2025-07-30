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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletChanged, int32, NewBulletCount);

UCLASS()
class TEAM01_API ACSPlayerCharacter : public ACSCharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter
	
public:
	ACSPlayerCharacter();

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

	UFUNCTION(BlueprintCallable)
	void Reload();

public:
	UPROPERTY(BlueprintAssignable)
	FOnBulletChanged OnBulletChanged;

private:
	void InputShoot(const FInputActionValue& InValue);
	void InputReload(const FInputActionValue& InValue);

	bool ConsumeBullet();

protected:
	int32 Bullet;
	int32 MaxBullet;
	
#pragma endregion

#pragma region Animation

public:
	virtual void BeginAttack() override;
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ShootMontage;

	FOnMontageEnded OnShootMontageEndedDelegate;
	
#pragma endregion

};
