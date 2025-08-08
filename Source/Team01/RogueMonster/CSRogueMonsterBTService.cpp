#include "CSRogueMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "CSRogueMonsterAIController.h"
#include "CSRogueMonsterBTDecorator.h"
#include "CSRogueMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCSRogueMonsterBTService::UCSRogueMonsterBTService()
{
	NodeName = TEXT("Check Player In Range");
	Interval = 0.5f;
}

void UCSRogueMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	ACSRogueMonster* Monster = Cast<ACSRogueMonster>(ControlledPawn);
	if (!Monster) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	const bool bInAttackRange = Distance <= Monster->AttackRange;
	Blackboard->SetValueAsBool(TEXT("IsInAttackRange"), bInAttackRange);

	const bool bInRangedAttackRange = Distance <= Monster->RangedAttackRange && Distance > Monster->AttackRange;
	Blackboard->SetValueAsBool(TEXT("IsInRangedAttackRange"), bInRangedAttackRange);

	const bool bDetectedPlayer = Distance <= Monster->SightRange;
	Blackboard->SetValueAsBool(TEXT("DetectedPlayer"), bDetectedPlayer);

	const bool bIsOverlap = Monster->bIsOverlap;
	Blackboard->SetValueAsBool(TEXT("bIsOverlap"), bIsOverlap);

	// 속도 전환
	if (bInAttackRange)
	{
		Monster->GetCharacterMovement()->MaxWalkSpeed = Monster->CloseRangeSpeed;
	}
	else if (bInRangedAttackRange)
	{
		Monster->GetCharacterMovement()->MaxWalkSpeed = Monster->ChaseSpeed;
	}
	else
	{
		Monster->GetCharacterMovement()->MaxWalkSpeed = Monster->PatrolSpeed;
	}


	//타겟 전환
	if (bDetectedPlayer)
	{
		Blackboard->SetValueAsObject(TEXT("TargetActor"), Player);
		Blackboard->ClearValue(TEXT("PatrolTarget"));

		Monster->bIsDetectedPlayer = true;
	}
	else
	{
		Monster->bIsDetectedPlayer = false;

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
			ACSRogueMonster* MonsterTarget = Cast<ACSRogueMonster>(ControlledPawn);
			if (MonsterTarget && MonsterTarget->PatrolPoints.Num() > 0)
			{
				AActor* NewTarget = Monster->PatrolPoints[FMath::RandRange(0, Monster->PatrolPoints.Num() - 1)];
				Blackboard->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
			}
		}
	}
}
