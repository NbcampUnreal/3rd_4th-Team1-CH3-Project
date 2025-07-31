#include "CSBTTask_BossFindPatrolLocation.h"
#include "Team01/BossMonster/CSBossMonster.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UCSBTTask_BossFindPatrolLocation::UCSBTTask_BossFindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UCSBTTask_BossFindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr) return EBTNodeResult::Failed;

	ACSBossMonster* BossMonster = Cast<ACSBossMonster>(ControllingPawn);
	if (BossMonster == nullptr) return EBTNodeResult::Failed;

	if (BossMonster->PatrolPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolPoints are not set."));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return EBTNodeResult::Failed;

	int32 CurrentPatrolIndex = BlackboardComp->GetValueAsInt(TEXT("PatrolIndex"));

	AActor* NextPatrolPoint = BossMonster->PatrolPoints[CurrentPatrolIndex];
	if (NextPatrolPoint == nullptr) return EBTNodeResult::Failed;

	BlackboardComp->SetValueAsVector(TEXT("PatrolLocation"), NextPatrolPoint->GetActorLocation());

	int32 NextPatrolIndex = (CurrentPatrolIndex + 1) % BossMonster->PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(TEXT("PatrolIndex"), NextPatrolIndex);

	return EBTNodeResult::Succeeded;
}