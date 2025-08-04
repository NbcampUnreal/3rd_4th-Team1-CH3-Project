#include "CSHiddenMonsterAnimInstance.h"
#include "CSHiddenMonster.h"

void UCSHiddenMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* OwnerPawn = TryGetPawnOwner();
    if (!OwnerPawn) return;

    Speed = OwnerPawn->GetVelocity().Size();

    ACSHiddenMonster* Monster = Cast<ACSHiddenMonster>(OwnerPawn);
    if (Monster)
    {
        bIsAttack = Monster->bIsAttack;
        bIsDead = Monster->bIsDead;
    }
}