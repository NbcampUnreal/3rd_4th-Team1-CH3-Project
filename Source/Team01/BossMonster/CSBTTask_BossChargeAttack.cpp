#include "CSBTTask_BossChargeAttack.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "Team01/BossMonster/CSBossMonster.h"

UCSBTTask_BossChargeAttack::UCSBTTask_BossChargeAttack()
{
    NodeName = TEXT("Boss Charge Attack"); // 비헤이비어 트리 에디터에 표시될 이름
}

EBTNodeResult::Type UCSBTTask_BossChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ACSBossMonster* BossMonster = Cast<ACSBossMonster>(AIController->GetPawn());
    if (BossMonster == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    BossMonster->BeginChargeAttack();

    // 태스크는 "재생하라는 명령을 내렸다"는 임무를 완수했으므로 '성공'을 반환합니다.
    return EBTNodeResult::Succeeded;
}