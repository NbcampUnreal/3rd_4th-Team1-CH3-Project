#include "CSMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
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
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
	}
}