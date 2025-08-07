#include "CSRogueMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	Blackboard->SetValueAsBool(TEXT("IsinAttackRange"), bInAttackRange);

	const bool bDetectedPlayer = Distance <= Monster->SightRange;
	Blackboard->SetValueAsBool(TEXT("DetectedPlayer"), bDetectedPlayer);

}
