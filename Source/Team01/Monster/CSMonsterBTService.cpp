#include "CSMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CSMonsterAIController.h"
#include "CSMonster.h"
#include "CSMonsterTargetPoint.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSMonsterBTService::UCSMonsterBTService()
{
	NodeName = TEXT("Check Player In Range");
	Interval = 0.5f;
}

void UCSMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	ACSMonster* Monster = Cast<ACSMonster>(ControlledPawn);
	if (!Monster) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	const bool bInAttackRange = Distance <= Monster->AttackRange;
	Blackboard->SetValueAsBool(TEXT("IsinAttackRange"), bInAttackRange);

	const bool bDetectedPlayer = Distance <= Monster->SightRange;
	Blackboard->SetValueAsBool(TEXT("DetectedPlayer"), bDetectedPlayer);

	if (bDetectedPlayer)
	{
		Blackboard->SetValueAsObject(TEXT("TargetActor"), Player);
		Blackboard->ClearValue(TEXT("PatrolTarget"));
	}
	else
	{
		Blackboard->ClearValue(TEXT("TargetActor"));

		AActor* CurrentPatrolTarget = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("PatrolTarget")));

		bool bNeedNewTarget = !CurrentPatrolTarget;
		if (CurrentPatrolTarget)
		{
			const float DistanceToPatrol = FVector::Dist(ControlledPawn->GetActorLocation(), CurrentPatrolTarget->GetActorLocation());
			bNeedNewTarget |= (DistanceToPatrol < 100.0f);
		}

		if (bNeedNewTarget)
		{
			TArray<AActor*> PatrolPoints;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSMonsterTargetPoint::StaticClass(), PatrolPoints);

			if (PatrolPoints.Num() > 0)
			{
				AActor* NewTarget = PatrolPoints[FMath::RandRange(0, PatrolPoints.Num() - 1)];
				Blackboard->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
			}
		}
	}

	if (Monster->GetCurrentHP() <= 0.f)
	{
		Monster->Die();
		return;
	}

	if (Monster->AttackRange < Distance)
	{
		Monster->EndAttack(nullptr, true);
	}
}