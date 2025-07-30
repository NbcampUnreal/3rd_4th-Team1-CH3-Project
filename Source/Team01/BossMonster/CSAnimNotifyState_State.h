#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Team01/Character/CSCharacterBase.h"
#include "CSAnimNotifyState_State.generated.h"


UCLASS()
class TEAM01_API UCSAnimNotifyState_State : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// 이 Notify State가 시작될 때 호출될 함수
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	// 이 Notify State가 끝날 때 호출될 함수
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	// 애니메이션 몽타주에서 설정할 상태 값을 에디터에 노출시킵니다.
	UPROPERTY(EditAnywhere, Category = "State")
	ECharacterState StateToSet;
};
