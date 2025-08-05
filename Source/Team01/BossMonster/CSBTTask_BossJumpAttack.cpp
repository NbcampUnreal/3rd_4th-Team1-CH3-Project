#include "CSBTTask_BossJumpAttack.h"
#include "AIController.h"
#include "Team01/BossMonster/CSBossMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCSBTTask_BossJumpAttack::UCSBTTask_BossJumpAttack()
{
	NodeName = TEXT("Boss Jump Attack");
}

EBTNodeResult::Type UCSBTTask_BossJumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACSBossMonster* BossMonster = Cast<ACSBossMonster>(OwnerComp.GetAIOwner()->GetPawn());
	AActor* TargetPlayer = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if (!BossMonster || !TargetPlayer)
	{
		return EBTNodeResult::Failed;
	}

	// 플레이어를 향해 방향 회전
	FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - BossMonster->GetActorLocation()).GetSafeNormal();
	FRotator LookAtRotation = DirectionToPlayer.Rotation();
	BossMonster->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

	//플레이어 방향으로 점프할 힘(Velocity)을 계산
	FVector LaunchVelocity = (DirectionToPlayer * BossMonster->GroundSlamJumpXYPower) + FVector(0, 0, BossMonster->GroundSlamJumpZPower);
	
	//점프
	BossMonster->LaunchCharacter(LaunchVelocity, true, true);

	//점프와 동시에 GroundSlam 애니메이션 재생.
	BossMonster->BeginAttackPattern(EBossAttackType::GroundSlam);

	return EBTNodeResult::Succeeded;
}