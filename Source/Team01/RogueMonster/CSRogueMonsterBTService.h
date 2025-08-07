#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CSRogueMonsterBTService.generated.h"

UCLASS()
class TEAM01_API UCSRogueMonsterBTService : public UBTService
{
	GENERATED_BODY()
	
public:
	UCSRogueMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
