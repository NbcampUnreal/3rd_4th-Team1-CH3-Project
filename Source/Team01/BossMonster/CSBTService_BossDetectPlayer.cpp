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
}

void UCSBTService_BossDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("Detect Player 서비스 실행 중!"));

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 1. 이 AI가 조종하는 폰(보스 몬스터)을 가져옵니다.
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return;
	}

	// 2. 월드에 있는 플레이어 폰을 가져옵니다.
	APawn* TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (TargetPlayer == nullptr)
	{
		return;
	}

	// 3. 블랙보드에 'TargetPlayer' 키의 값을 방금 찾은 플레이어로 업데이트합니다.
	// GetSelectedBlackboardKey()는 에디터에서 우리가 지정할 키(TargetPlayer)를 의미합니다.
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), TargetPlayer);

	// 4. 보스와 플레이어 사이의 거리를 계산합니다.
	const float DistanceToPlayer = ControllingPawn->GetDistanceTo(TargetPlayer);

	// 5. 계산된 거리에 따라 'IsInAttackRange' 키의 값을 true 또는 false로 업데이트합니다.
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsInAttackRange"), DistanceToPlayer <= AttackRange);

	ACSCharacterBase* ControllingCharacter = Cast<ACSCharacterBase>(ControllingPawn);
	if (ControllingCharacter == nullptr) return;

	// 6. 캐릭터의 현재 C++ 상태(CurrentState)를 가져옵니다.
	ECharacterState CurrentPawnState = ControllingCharacter->GetCurrentState();

	// 7. 블랙보드의 'CurrentState' 키 값을 방금 가져온 캐릭터의 상태 값으로 업데이트합니다.
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), (uint8)CurrentPawnState);
}