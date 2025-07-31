#include "CSBTService_BossDetectPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Team01/Character/CSCharacterBase.h"
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

	// 플레이어를 인식하는 거리 2000cm (20m)
	DetectRange = 2000.0f; 
}

void UCSBTService_BossDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("Detect Player 서비스 실행 중!"));

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

    const float DistanceToPlayer = ControllingPawn->GetDistanceTo(TargetPlayer);

    // ▼▼ 핵심 로직: 인식 범위 안에 있을 때와 아닐 때를 구분함 ▼▼
    if (DistanceToPlayer <= DetectRange)
    {
        // [범위 안] 플레이어를 타겟으로 설정하고, 공격 범위 여부를 업데이트
        BlackboardComp->SetValueAsObject(TEXT("TargetPlayer"), TargetPlayer);
        BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), DistanceToPlayer <= AttackRange);
    }
    else
    {
        // [범위 밖] 플레이어를 잊어버리고(타겟을 비움), 공격 범위도 false로 설정
        BlackboardComp->ClearValue(TEXT("TargetPlayer"));
        BlackboardComp->SetValueAsBool(TEXT("IsInAttackRange"), false);
    }
}