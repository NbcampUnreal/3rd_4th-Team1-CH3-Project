#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSCharacterBase.generated.h"


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
	bool GetIsDead() const { return bIsDead; }
	UFUNCTION(BlueprintCallable)
	float GetMaxHP() const { return MaxHP; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() const { return CurrentHP; }
	UFUNCTION(BlueprintCallable)
	float GetAttackDamage() const { return AttackDamage; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackDamage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsAttackKeyPressed : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsNowAttacking : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsDead : 1;
	
#pragma endregion
	
};
