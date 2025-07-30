#include "CSMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSMonsterAIController.h"
#include "AIController.h"

ACSMonster::ACSMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSMonsterAIController::StaticClass();

	bIsDead = false;
	bIsAttack = false;

	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	AttackDamage = 10.0f;

	SightRange = 1500.0f;
	AttackRange = 400.0f;
}

void ACSMonster::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn) return;

	const float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());

	if (Distance <= AttackRange)
	{
		BeginAttack();
	}
	else if (Distance <= SightRange)
	{
		ChasePlayer();
	}

}

void ACSMonster::BeginAttack()
{
	Super::BeginAttack();

	if (bIsDead || bIsAttack) return;

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
