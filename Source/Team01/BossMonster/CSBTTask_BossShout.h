#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CSBTTask_BossShout.generated.h"

UCLASS()
class TEAM01_API UCSBTTask_BossShout : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCSBTTask_BossShout();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 에디터에서 샤우팅 몽타주를 지정
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> ShoutMontage;

};
