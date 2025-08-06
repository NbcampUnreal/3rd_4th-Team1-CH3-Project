#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CSMummyMonsterBTService.generated.h"

UCLASS()
class TEAM01_API UCSMummyMonsterBTService : public UBTService
{
	GENERATED_BODY()
	
public:
	UCSMummyMonsterBTService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


};
