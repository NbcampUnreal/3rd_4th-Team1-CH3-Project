#include "CSBTTask_BossShout.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UCSBTTask_BossShout::UCSBTTask_BossShout()
{
	NodeName = TEXT("Boss Shout");
}

EBTNodeResult::Type UCSBTTask_BossShout::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACSCharacterBase* ControllingPawn = Cast<ACSCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (ControllingPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* TargetPlayer = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
	if (TargetPlayer == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 1. 플레이어를 향해 부드럽게 회전
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ControllingPawn->GetActorLocation(), TargetPlayer->GetActorLocation());
	ControllingPawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

	// 2. 상태를 'Shouting'으로 설정하여 다른 행동(이동, 피격)을 막음
	ControllingPawn->SetCurrentState(ECharacterState::Shouting);

	// 3. 샤우팅 몽타주를 재생하고, 몽타주가 끝나면 상태를 Idle로 되돌림
	UAnimInstance* AnimInstance = ControllingPawn->GetMesh()->GetAnimInstance();
	if (AnimInstance && ShoutMontage)
	{
		// 몽타주가 끝나면 호출될 델리게이트(Lambda)를 설정합니다.
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindLambda([ControllingPawn](UAnimMontage* Montage, bool bInterrupted)
			{
				// 몽타주가 끝나면 상태를 Idle로 되돌립니다.
				ControllingPawn->SetCurrentState(ECharacterState::Idle);
			});

		AnimInstance->Montage_Play(ShoutMontage);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ShoutMontage);
	}
	else
	{
		// 몽타주가 없으면 바로 Idle로 돌아가고 태스크는 실패 처리
		ControllingPawn->SetCurrentState(ECharacterState::Idle);
		return EBTNodeResult::Failed;
	}

	// 4. 다시는 이 행동을 하지 않도록 bIsFirstEncounter를 false로 설정
	BlackboardComp->SetValueAsBool(TEXT("bIsFirstEncounter"), false);

	return EBTNodeResult::Succeeded;
}