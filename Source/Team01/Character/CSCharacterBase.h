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
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ShootMontage;

#pragma endregion

#pragma region Status

public:
	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const { return bIsDead; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsNowAttacking : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsAttackKeyPressed : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsDead : 1;
	
#pragma endregion
	
};
