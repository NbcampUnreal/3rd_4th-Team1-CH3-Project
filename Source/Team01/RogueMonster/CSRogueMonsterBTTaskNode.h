#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSRogueMonsterBTTaskNode.generated.h"

UCLASS()
class TEAM01_API UCSRogueMonsterBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UCSRogueMonsterBTTaskNode();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
