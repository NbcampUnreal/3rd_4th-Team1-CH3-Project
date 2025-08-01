#include "CSAnimNotify_BossAttackHitCheck.h"
#include "Team01/BossMonster/CSBossMonster.h"

void UCSAnimNotify_BossAttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 이 애니메이션의 주인을 찾아 AttackHitCheck 함수를 호출
	if (ACSBossMonster* BossMonster = Cast<ACSBossMonster>(MeshComp->GetOwner()))
	{
		BossMonster->AttackHitCheck();
	}
}