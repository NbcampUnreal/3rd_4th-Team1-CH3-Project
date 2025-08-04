#include "CSBTTask_BossShout.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UCSBTTask_BossShout::UCSBTTask_BossShout()
{
	NodeName = TEXT("Boss Shout");
}

EBTNodeResult::Type UCSBTTask_BossShout::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACharacter* ControllingPawn = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (ControllingPawn == nullptr) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return EBTNodeResult::Failed;

	ACharacter* TargetPlayer = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
	if (TargetPlayer == nullptr) return EBTNodeResult::Failed;

	// 1. 플레이어를 향해 부드럽게 회전합니다.
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ControllingPawn->GetActorLocation(), TargetPlayer->GetActorLocation());
	ControllingPawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

	// 2. 샤우팅 몽타주를 재생합니다.
	if (ShoutMontage)
	{
		ControllingPawn->PlayAnimMontage(ShoutMontage);
	}

	// 3. 다시는 이 행동을 하지 않도록 bIsFirstEncounter를 false로 설정합니다.
	BlackboardComp->SetValueAsBool(TEXT("bIsFirstEncounter"), false);

	// 4. 태스크 성공을 알립니다.
	return EBTNodeResult::Succeeded;
}