#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CSBTTask_BossJumpAttack.generated.h"

UCLASS()
class TEAM01_API UCSBTTask_BossJumpAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UCSBTTask_BossJumpAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};