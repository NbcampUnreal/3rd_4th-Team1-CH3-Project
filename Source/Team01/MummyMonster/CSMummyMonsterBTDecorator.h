#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSMummyMonsterBTDecorator.generated.h"

UENUM(BlueprintType)
enum class EMummyMonsterAIConditionType : uint8
{
	IsDead
};

UCLASS()
class TEAM01_API UCSMummyMonsterBTDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCSMummyMonsterBTDecorator();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const override;

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	EMummyMonsterAIConditionType ConditionType;
	
};
