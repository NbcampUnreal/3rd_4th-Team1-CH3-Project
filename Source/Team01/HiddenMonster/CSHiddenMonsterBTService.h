#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CSHiddenMonsterBTService.generated.h"

UCLASS()
class TEAM01_API UCSHiddenMonsterBTService : public UBTService
{
	GENERATED_BODY()

public:
	UCSHiddenMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
