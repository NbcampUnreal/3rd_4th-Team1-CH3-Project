#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CSBTTask_BossFindPatrolLocation.generated.h"

UCLASS()
class TEAM01_API UCSBTTask_BossFindPatrolLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCSBTTask_BossFindPatrolLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
