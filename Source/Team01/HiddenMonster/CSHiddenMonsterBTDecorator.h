#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSHiddenMonsterBTDecorator.generated.h"

UENUM(BlueprintType)
enum class EHiddenMonsterAIConditionType : uint8
{
	IsDead,
	IsAttacking
};

UCLASS()
class TEAM01_API UCSHiddenMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCSHiddenMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;
	
public:
	UPROPERTY(EditAnywhere)
	EHiddenMonsterAIConditionType ConditionType;
};
