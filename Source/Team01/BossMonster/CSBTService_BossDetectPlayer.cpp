#include "CSBTService_BossDetectPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Team01/Character/CSCharacterBase.h"
#include "Team01/BossMonster/CSBossMonster.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

UCSBTService_BossDetectPlayer::UCSBTService_BossDetectPlayer()
{
	// 에디터의 비헤이비어 트리에서 이 서비스의 이름을 설정
	NodeName = TEXT("Boss Detect Player");

	// 이 서비스가 실행될 주기 (0.5초)
	Interval = 0.5f;

	// 실행 주기에 약간의 무작위성을 부여합니다. (여러 AI가 동시에 계산하는 것을 방지)
	RandomDeviation = 0.1f;

	// 기본 공격 사정거리를 300cm (3m)로 설정, 에디터에서 수정 가능
	AttackRange = 300.0f;

    // 점프 공격 사정거리 1000cm
    JumpAttackRange = 1000.0f;

	// 플레이어를 인식하는 거리 2000cm (20m)
	DetectRange = 2000.0f; 
}

void UCSBTService_BossDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ControllingPawn == nullptr) return;

    APawn* TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (TargetPlayer == nullptr) return;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr) return;

    // C++ 캐릭터의 현재 상태를 블랙보드에 업데이트
    ACSCharacterBase* ControllingCharacter = Cast<ACSCharacterBase>(ControllingPawn);
    if (ControllingCharacter)
    {
        BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), (uint8)ControllingCharacter->GetCurrentState());
    }

    if (ACSBossMonster* Boss = Cast<ACSBossMonster>(ControllingPawn))
    {
        BlackboardComp->SetValueAsBool(TEXT("bIsInPhase2"), Boss->GetIsInPhase2());
    }

    const float DistanceToPlayer = ControllingPawn->GetDistanceTo(TargetPlayer);

    // 블랙보드에서 "플레이어를 본 적 있는가?" 값을 가져옵니다.
    bool bHasBeenAlerted = BlackboardComp->GetValueAsBool(TEXT("bHasBeenAlerted"));

    // 아직 플레이어를 본 적이 없고(false), 현재 인식 범위 안에 들어왔다면
    if (!bHasBeenAlerted && DistanceToPlayer <= DetectRange)
    {
        // "이제 플레이어를 봤다!" 라고 블랙보드에 영구적으로 기록
        bHasBeenAlerted = true;
        BlackboardComp->SetValueAsBool(TEXT("bHasBeenAlerted"), true);
    }

    // 플레이어를 한 번이라도 본 적이 있다면(true)
    if (bHasBeenAlerted)
    {
        // 인식 범위를 벗어나더라도 계속 플레이어를 타겟으로 설정합니다.
        BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), TargetPlayer);
        BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), DistanceToPlayer <= AttackRange);
        BlackboardComp->SetValueAsBool(TEXT("IsInJumpAttackRange"), DistanceToPlayer <= JumpAttackRange);
    }
    // 단 한 번도 플레이어를 본 적이 없다면 (최초 순찰 상태)
    else
    {
        // 타겟을 비우고, 공격 범위도 모두 false로 설정합니다.
        BlackboardComp->ClearValue(TEXT("TargetPlayer"));
        BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), false);
        BlackboardComp->SetValueAsBool(TEXT("IsInJumpAttackRange"), false);
    }
}