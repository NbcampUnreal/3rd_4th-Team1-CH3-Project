#include "CSCharacterBase.h"
#include "../Character/Controller/CSPlayerController.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "Animation/CSPlayerAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

int32 ACSCharacterBase::ShowAttackRangedDebug = 0;

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

	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance))
	{
		AnimInstance->OnPostDead.AddDynamic(this, &ThisClass::HandleOnPostDead);
	}
}

void ACSCharacterBase::HandleOnCheckInputAttack()
{
}

// void ACSCharacterBase::HandleOnCheckHit()
// {
// }

void ACSCharacterBase::BeginAttack()
{
}

void ACSCharacterBase::EndAttack(UAnimMontage* InMontage, bool bInterrupted)
{
}

float ACSCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("%s was taken damage: %.f"), *GetName(), FinalDamageAmount);
	
	CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);

	if (IsValid(DamageCauser))
	{
		APawn* InstigatorPawn = Cast<APawn>(DamageCauser);
		if (InstigatorPawn && InstigatorPawn->IsPlayerControlled())
		{
			APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
			if (IsValid(PC))
			{
				ACSPlayerController* MyPC = Cast<ACSPlayerController>(PC);
				if (MyPC && MyPC->GetHUDWidget())
				{
					MyPC->GetHUDWidget()->ShowHitMarker();
				}
			}
		}
	}
	
	if (IsPlayerControlled())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (IsValid(PC))
		{
			ACSPlayerController* MyPC = Cast<ACSPlayerController>(PC);
			if (MyPC && MyPC->GetHUDWidget())
			{
				MyPC->GetHUDWidget()->UpdateHP(CurrentHP / MaxHP);
			}
		}
	}
	
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		SetCurrentState(ECharacterState::Dead);
		CurrentHP = 0.f;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(MOVE_None);
	}

	if (!IsPlayerControlled())
	{
		if (UWidgetComponent* WidgetComp = FindComponentByClass<UWidgetComponent>())
		{
			if (UUserWidget* Widget = WidgetComp->GetUserWidgetObject())
			{
				if (UCS_WBP_EnemyHPBar* EnemyHP = Cast<UCS_WBP_EnemyHPBar>(Widget))
				{
					EnemyHP->SetHPBarPercent(CurrentHP / MaxHP);
				}
			}
		}
	}
	
	return FinalDamageAmount;
}

void ACSCharacterBase::SetCurrentState(ECharacterState NewState)
{
	// 새로운 상태로 현재 상태를 변경합니다.
	CurrentState = NewState;

	UE_LOG(LogTemp, Log, TEXT("%s 님의 상태가 %d 로 변경"), *GetName(), NewState);
}

bool ACSCharacterBase::GetIsDead() const
{
	// bIsDead 변수 대신 CurrentState를 확인하여 사망 여부를 확인
	return CurrentState == ECharacterState::Dead;
}

void ACSCharacterBase::HandleOnPostDead()
{
	SetLifeSpan(0.1f);
}
