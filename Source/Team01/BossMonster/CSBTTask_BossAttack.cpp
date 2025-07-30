#include "CSBTTask_BossAttack.h"
#include "AIController.h"
#include "Team01/BossMonster/CSBossMonster.h"

UCSBTTask_BossAttack::UCSBTTask_BossAttack()
{
	NodeName = TEXT("Boss Attack"); // 비헤이비어 트리 에디터에 표시될 이름
}

EBTNodeResult::Type UCSBTTask_BossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// 1. 이 태스크를 실행한 AI의 컨트롤러를 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed; // 실패
	}

	// 2. AI 컨트롤러가 조종하는 폰(보스 몬스터)을 가져옵니다.
	ACSBossMonster* BossMonster = Cast<ACSBossMonster>(AIController->GetPawn());
	if (BossMonster == nullptr)
	{
		return EBTNodeResult::Failed; // 실패
	}

	// 3. 보스 몬스터의 공격 함수를 호출합니다!
	BossMonster->BeginAttack();

	// 4. 태스크가 성공적으로 완료되었다고 비헤이비어 트리에 알립니다.
	return EBTNodeResult::Succeeded;
}