#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSMonsterBTTaskNode.generated.h"

UCLASS()
class TEAM01_API UCSMonsterBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCSMonsterBTTaskNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
