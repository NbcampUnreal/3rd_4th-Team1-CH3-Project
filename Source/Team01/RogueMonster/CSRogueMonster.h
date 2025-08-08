#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CSRogueMonster.generated.h"

UCLASS()
class TEAM01_API ACSRogueMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ACSRogueMonster();

protected:
	virtual void BeginPlay() override;

public:
	float MaxHP;
	float CurrentHP;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float HitDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float RangedHitDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI|Detection")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection")
	float RangedAttackRange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "AI|Detection")
	float SightRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float CloseRangeSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool bIsOverlap;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAttack;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRangeAttack;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHit;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDetectedPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = "Ranged")
	TSubclassOf<AActor> DaggerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY()
	APawn* PlayerPawn;

public:

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void ApplyAttackDamage();

	void LookAtPlayer();

	void BeginAttack();

	void BeginRangeAttack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	void Die();

	UFUNCTION()
	void ResetOverlap();

	FTimerHandle OverlapResetTimerHandle;
};
