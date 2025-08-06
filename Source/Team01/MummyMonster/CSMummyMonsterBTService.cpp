#include "CSMummyMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CSMummyAIController.h"
#include "CSMummyMonsterBTDecorator.h"
#include "CSMummyMonster.h"
#include "Team01/Monster/CSMonsterTargetPoint.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Team01/Monster/CSMonsterTargetPoint.h"

UCSMummyMonsterBTService::UCSMummyMonsterBTService()
{
	NodeName = TEXT("Check PatrolTarget In Range");
	Interval = 0.5f;
}

void UCSMummyMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	ACSMummyMonster* Monster = Cast<ACSMummyMonster>(ControlledPawn);
	if (!Monster) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;


	const bool bIsDead = Monster->GetCurrentHP() <= 0;
	Blackboard->SetValueAsBool(TEXT("IsDead"), bIsDead);

	AActor* CurrentPatrolTarget = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("PatrolTarget")));

	bool bNeedNewTarget = !CurrentPatrolTarget;
	if (CurrentPatrolTarget)
	{
		const float DistanceToPatrol = FVector::Dist(ControlledPawn->GetActorLocation(), CurrentPatrolTarget->GetActorLocation());
		bNeedNewTarget |= (DistanceToPatrol < 100.0f);
	}

	if (bNeedNewTarget)
	{
		ACSMummyMonster* MonsterTarget = Cast<ACSMummyMonster>(ControlledPawn);
		if (MonsterTarget && MonsterTarget->PatrolPoints.Num() > 0)
		{
			AActor* NewTarget = Monster->PatrolPoints[FMath::RandRange(0, Monster->PatrolPoints.Num() - 1)];
			Blackboard->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
		}
	}
}