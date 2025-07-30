#include "CSBossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

ACSBossAIController::ACSBossAIController()
{
	// 생성자에서 비헤이비어 트리와 블랙보드 에셋을 로드 할 수 있지만
	// 에디터에서 진행
}

void ACSBossAIController::OnPossess(APawn* InPawn) //빙의 되었을 때
{
	Super::OnPossess(InPawn);

	//블랙보드 컴포넌트 초기화, 비헤이비어 트리 실행

	if (BossBehaviorTree)
	{
		RunBehaviorTree(BossBehaviorTree);

		UE_LOG(LogTemp, Log, TEXT("보스 비헤이비어 트리 실행!"));
	}
}
