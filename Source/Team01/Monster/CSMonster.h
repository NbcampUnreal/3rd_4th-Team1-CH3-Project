#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "CSMonster.generated.h"

UCLASS()
class TEAM01_API ACSMonster : public ACSCharacterBase
{
	GENERATED_BODY()

public:	
	ACSMonster();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void BeginAttack() override;
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

	void Die();
	float GetCurrentHP();

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttack;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHit;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float SightRange;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float AttackRange;

	UPROPERTY(EditInstanceOnly, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

protected:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> HPBarComponent;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dead")
	UAnimMontage* DeadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	UAnimMontage* HitMontage;

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY()
	APawn* PlayerPawn;
	
};
