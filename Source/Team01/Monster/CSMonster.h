#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "../Ui/CS_WBP_EnemyHPBar.h"
#include "Components/WidgetComponent.h"
#include "CSMonster.generated.h"

class UStaticMeshComponent;

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

	UPROPERTY(BlueprintReadWrite)
	bool bIsDetectedPlayer;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float SightRange;

	UPROPERTY(EditAnywhere, Category = "Monster|Range")
	float AttackRange;

	UPROPERTY(EditInstanceOnly, Category = "Patrol Point")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = "AI")
	UStaticMeshComponent* SightCone;

protected:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> HPBarComponent;

	UPROPERTY()
	TObjectPtr<UCS_WBP_EnemyHPBar> HPBar;

	UPROPERTY()
	AController* LastInstigator = nullptr;
private:

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> ThrownActorClass;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ThrowActor();

	UPROPERTY()
	APawn* PlayerPawn;
	
};
