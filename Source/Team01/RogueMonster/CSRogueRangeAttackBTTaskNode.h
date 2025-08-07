#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSRogueRangeAttackBTTaskNode.generated.h"

UCLASS()
class TEAM01_API UCSRogueRangeAttackBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

	UCSRogueRangeAttackBTTaskNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
