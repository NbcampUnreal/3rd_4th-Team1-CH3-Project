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

	bIsDead = false;
	bIsAttack = false;

	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	AttackDamage = 10.0f;

	SightRange = 800.0f;
	AttackRange = 150.0f;

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

float ACSMonster::GetCurrentHP()
{
	return CurrentHP;
}

void ACSMonster::BeginAttack()
{
	Super::BeginAttack();

	if (GetCurrentState() != ECharacterState::Idle) return;

	SetCurrentState(ECharacterState::Attacking);

	if (PlayerPawn)
	{
		FRotator LookAtRotation = (PlayerPawn->GetActorLocation() - GetActorLocation()).Rotation();
		FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

		SetActorRotation(TargetRotation);
	}

	AAIController* AIController = Cast<AAIController>(GetController());

	if (AIController)
	{
		AIController->StopMovement();
	}

	bIsAttack = true;

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void ACSMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	Super::EndAttack(InMontage, bInterruped);

	bIsAttack = false;

	SetCurrentState(ECharacterState::Idle);
}

//void ACSMonster::ChasePlayer()
//{
//	if (bIsAttack || bIsDead) return;
//
//	AAIController* AIController = Cast<AAIController>(GetController());
//	if (AIController && PlayerPawn)
//	{
//		AIController->MoveToActor(PlayerPawn);
//	}
//}

void ACSMonster::Die()
{
	AAIController* AIController = Cast<AAIController>(GetController());

	bIsDead = true;

	AIController->UnPossess();

	SetCurrentState(ECharacterState::Dead);

	if (DeadMontage)
	{
		PlayAnimMontage(DeadMontage);
	}
}
