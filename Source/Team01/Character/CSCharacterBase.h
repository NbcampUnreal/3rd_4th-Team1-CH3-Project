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

	virtual void BeginAttack();

	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

#pragma endregion

#pragma region Status

protected:
	bool bIsNowAttacking;
	bool bIsAttackKeyPressed;
	bool bIsDead;
	
#pragma endregion
	
};
