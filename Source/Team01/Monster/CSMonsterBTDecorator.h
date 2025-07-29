#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSMonsterBTDecorator.generated.h"

UENUM(BlueprintType)
enum class EAIConditionType : uint8
{
	IsDead,
	IsNotDead,
	IsAttacking,
	IsNotAttacking
};

UCLASS()
class TEAM01_API UCSMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCSMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	EAIConditionType ConditionType;
	
};
