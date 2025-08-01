#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CSAnimNotify_BossAttackHitCheck.generated.h"

UCLASS()
class TEAM01_API UCSAnimNotify_BossAttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()

public:
	//Notify 실행 함수
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
