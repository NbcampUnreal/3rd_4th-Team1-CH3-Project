#include "CSBossAnimInstance.h"
#include "Team01/Character/CSCharacterBase.h"

UCSBossAnimInstance::UCSBossAnimInstance()
{
	Speed = 0.0f;
}

void UCSBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// OwningCharacter 변수가 유효한지 확인합니다.
	if (!OwningCharacter)
	{
		// 유효하지 않다면, 이 애님 인스턴스의 폰을 가져와 저장
		OwningCharacter = Cast<ACSCharacterBase>(TryGetPawnOwner());
	}

	if (OwningCharacter)
	{
		// 캐릭터의 속도를 가져와 Speed 변수에 저장
		Speed = OwningCharacter->GetVelocity().Size();
	}
}