#include "CSCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/CSPlayerAnimInstance.h"

int32 ACSCharacterBase::ShowAttackRangedDebug = 1;

FAutoConsoleVariableRef CVarShowAttackRangedDebug(
	TEXT("CS.ShowAttackRangedDebug"),
	ACSCharacterBase::ShowAttackRangedDebug,
	TEXT(""),
	ECVF_Cheat
);

ACSCharacterBase::ACSCharacterBase()
{
	CurrentState = ECharacterState::Idle; // 게임이 시작될 때 캐릭터의 상태를 Idle로 초기화

}

void ACSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACSCharacterBase::HandleOnCheckInputAttack()
{
}

void ACSCharacterBase::HandleOnCheckHit()
{
}

void ACSCharacterBase::HandleOnPostDead()
{
}

void ACSCharacterBase::BeginAttack()
{
}

void ACSCharacterBase::EndAttack(UAnimMontage* InMontage, bool bInterrupted)
{
}

bool ACSCharacterBase::GetIsDead() const
{
	// bIsDead 변수 대신 CurrentState를 확인하여 사망 여부를 확인
	return CurrentState == ECharacterState::Dead;
}

void ACSCharacterBase::SetCurrentState(ECharacterState NewState)
{
	// 새로운 상태로 현재 상태를 변경합니다.
	CurrentState = NewState;

	UE_LOG(LogTemp, Log, TEXT("%s 님의 상태가 %d 로 변경"), *GetName(), NewState);
}
