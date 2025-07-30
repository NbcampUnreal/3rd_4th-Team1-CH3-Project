#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "CSBTTask_BossAttack.generated.h"

UCLASS()
class TEAM01_API UCSBTTask_BossAttack : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	UCSBTTask_BossAttack();

protected:
	// Task 실행될때 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

