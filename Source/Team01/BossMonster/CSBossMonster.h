#pragma once

#include "CoreMinimal.h"
#include "Team01/Character/CSCharacterBase.h"
#include "CSBossMonster.generated.h"


UCLASS()
class TEAM01_API ACSBossMonster : public ACSCharacterBase
{
	GENERATED_BODY()
	
public:
    ACSBossMonster();

protected:
    virtual void BeginPlay() override;
  
    virtual void BeginAttack() override;

    virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;
    
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
 
    void Die(); //죽었을 때 처리될 함수
};
