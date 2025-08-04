#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacterBase.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Attacking,
	HitReaction,
	Shouting,
	PhaseTransition,
	Dead
};

UCLASS()
class TEAM01_API ACSCharacterBase : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter
	
public:
	ACSCharacterBase();
	
protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Attack

public:
	UFUNCTION()
	void HandleOnCheckInputAttack();
	// UFUNCTION()
	// void HandleOnCheckHit();

	UFUNCTION()
	virtual void BeginAttack();
	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted);

	// 공통 TakeDamage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	static int32 ShowAttackRangedDebug;
	

#pragma endregion

#pragma region Status

public:
	UFUNCTION(BlueprintCallable)
	float GetMaxHP() const { return MaxHP; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() const { return CurrentHP; }
	UFUNCTION(BlueprintCallable)
	float GetAttackDamage() const { return AttackDamage; }

	//외부에서 현재상태를 확인하고 변경할 수 있는 함수 추가
	UFUNCTION(BlueprintCallable, Category = "State")
	ECharacterState GetCurrentState() const { return CurrentState; }
	void SetCurrentState(ECharacterState NewState);

	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const; //bIsDead 대신 상태를 확인 하도록 변경

	UFUNCTION()
	virtual void HandleOnPostDead();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackDamage;

	//현재 상태를 저장할 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECharacterState CurrentState;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsAttackKeyPressed : 1;

	// 'bIsNowAttacking'과 'bIsDead'는 CurrentState로 통합 관리되므로 제거합니다.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//uint8 bIsNowAttacking : 1;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//uint8 bIsDead : 1;
	
#pragma endregion
	
};
