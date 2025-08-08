#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "Team01/BossMonster/CSBossMonster.h"
#include "CSBTTask_BossChargeAttack.generated.h"

UCLASS()
class TEAM01_API UCSBTTask_BossChargeAttack : public UBTTask_BlueprintBase
{
	GENERATED_BODY()
	
public:
	UCSBTTask_BossChargeAttack();

protected:
	// Task 실행될때 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 비헤이비어 트리 에디터에서 이 태스크가 어떤 공격을 실행할지 선택할 수 있도록 변수를 노출
	UPROPERTY(EditAnywhere, Category = "Attack")
	EBossAttackType AttackTypeToPerform;
};
