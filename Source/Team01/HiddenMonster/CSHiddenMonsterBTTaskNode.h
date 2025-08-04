#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSHiddenMonsterBTTaskNode.generated.h"

UCLASS()
class TEAM01_API UCSHiddenMonsterBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCSHiddenMonsterBTTaskNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
