#include "CSAnimNotifyState_State.h"

void UCSAnimNotifyState_State::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	// 애니메이션이 재생되는 캐릭터를 가져옵니다.
	ACSCharacterBase* Character = Cast<ACSCharacterBase>(MeshComp->GetOwner());
	if (Character)
	{
		// 캐릭터의 상태를 에디터에서 설정한 값(StateToSet)으로 변경합니다.
		Character->SetCurrentState(StateToSet);
	}
}

void UCSAnimNotifyState_State::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	// 애니메이션이 재생되는 캐릭터를 가져옵니다.
	ACSCharacterBase* Character = Cast<ACSCharacterBase>(MeshComp->GetOwner());
	if (Character)
	{
		// 애니메이션이 끝나면 캐릭터의 상태를 다시 '평시(Idle)'로 돌려놓습니다.
		Character->SetCurrentState(ECharacterState::Idle);
	}
}