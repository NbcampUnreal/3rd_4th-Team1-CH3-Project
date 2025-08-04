#include "CSHiddenMonsterBTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CSHiddenMonster.h"
#include "GameFramework/Pawn.h"

UCSHiddenMonsterBTService::UCSHiddenMonsterBTService()
{
    NodeName = TEXT("Check Attack Condition");
    Interval = 0.3f;
}

void UCSHiddenMonsterBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return;

    APawn* Pawn = AICon->GetPawn();
    if (!Pawn) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (!TargetActor) return;

    ACSHiddenMonster* Monster = Cast<ACSHiddenMonster>(Pawn);
    if (!Monster) return;

    float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetActor->GetActorLocation());

    bool bCanAttack = Distance <= Monster->AttackRange;

    if (bCanAttack)
    {
        Monster->bIsAttack = true;
    }
    else
    {
        Monster->bIsAttack = false;
    }
}