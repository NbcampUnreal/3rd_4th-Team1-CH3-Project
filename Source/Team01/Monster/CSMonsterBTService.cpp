#include "CSMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
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

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Player->GetActorLocation());

	if (Distance <= DetectRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), Player);

		OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("PatrolTarget"));
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));


		AActor* CurrentPatrolTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("PatrolTarget")));

		if (!CurrentPatrolTarget || FVector::Dist(ControlledPawn->GetActorLocation(), CurrentPatrolTarget->GetActorLocation()) < 100.0f)
		{
			TArray<AActor*> PatrolPoints;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSMonsterTargetPoint::StaticClass(), PatrolPoints);

			if (PatrolPoints.Num() > 0)
			{
				AActor* NewTarget = PatrolPoints[FMath::RandRange(0, PatrolPoints.Num() - 1)];
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("PatrolTarget"), NewTarget);
			}
		}
	}
}