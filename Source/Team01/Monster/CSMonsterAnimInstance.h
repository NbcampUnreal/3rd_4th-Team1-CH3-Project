#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CSMonsterAnimInstance.generated.h"

UCLASS()
class TEAM01_API UCSMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	bool bIsDead;
	UPROPERTY(BlueprintReadOnly, Category = "Monster|Animation")
	bool bIsAttack;

	UPROPERTY()
	class APawn* OwnerPawn;
};
