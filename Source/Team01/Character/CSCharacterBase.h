#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacterBase.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,      // 평상시(가만히 있을때)
	Attacking, // 공격 중
	Dead       // 사망
	// 필요에 따라 '피격 중(Hit)', '돌진 중(Dashing)' 등등 추가 하시면 될 것 같습니다.
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
	UFUNCTION()
	void HandleOnCheckHit();
	UFUNCTION()
	void HandleOnPostDead();

	UFUNCTION()
	virtual void BeginAttack();
	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterrupted);

public:
	static int32 ShowAttackRangedDebug;
	

#pragma endregion

#pragma region Status

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const; //bIsDead 대신 상태를 확인 하도록 변경
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
