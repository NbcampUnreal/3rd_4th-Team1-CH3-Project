#include "CSMonster.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSMonsterAIController.h"
#include "AIController.h"

ACSMonster::ACSMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSMonsterAIController::StaticClass();

	// 삭제: bIsDead와 bIsAttack 변수는 CurrentState로 관리하게 되어서 false로 초기화 하지 않아도 됌
	// bIsDead = false;
	// bIsAttack = false;

	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	AttackDamage = 10.0f;

	SightRange = 1500.0f;
	AttackRange = 400.0f;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(150.f, 20.f));
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // 머리 위
}

void ACSMonster::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
	{
		UCS_WBP_EnemyHPBar* HPWidget = Cast<UCS_WBP_EnemyHPBar>(Widget);
		if (HPWidget)
		{
			HPWidget->SetHPBarPercent(CurrentHP / MaxHP);
		}
	}
}

void ACSMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSMonster::BeginAttack()
{
	Super::BeginAttack();

	// 변경: 여러 bool 변수를 확인하는 대신, 현재 상태가 '평시'가 아니면 공격하지 않도록 합니다.
	if (GetCurrentState() != ECharacterState::Idle) return;

	// 변경: 상태를 'Attacking'으로 설정합니다.
	SetCurrentState(ECharacterState::Attacking);

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void ACSMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndAttack(InMontage, bInterruped);

	// 변경: 상태를 다시 '평시(Idle)'로 설정합니다.
	SetCurrentState(ECharacterState::Idle);
}

void ACSMonster::ChasePlayer()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && PlayerPawn)
	{
		AIController->MoveToActor(PlayerPawn);
	}
}

void ACSMonster::Die()
{
}
