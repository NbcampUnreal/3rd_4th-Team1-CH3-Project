#include "../Animation/AN_CheckHit.h"
#include "../CSCharacterBase.h"

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		ACSCharacterBase* AttackingCharacter =
			Cast<ACSCharacterBase>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter))
		{
			AttackingCharacter->HandleOnCheckHit();
		}
	}
}
