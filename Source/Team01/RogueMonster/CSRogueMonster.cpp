#include "CSRogueMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSRogueMonsterAIController.h"
#include "CSRogueMonsterAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "../Character/Controller/CSPlayerController.h"

ACSRogueMonster::ACSRogueMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSRogueMonsterAIController::StaticClass();

	bIsDead = false;
	bIsAttack = false;
	bIsRangeAttack = false;
	bIsHit = false;
	bIsDetectedPlayer = false;

	MaxHP = 40.0f;
	CurrentHP = MaxHP;
	HitDamage = 20.0f;
	RangedHitDamage = 30.0f;

	PatrolSpeed = 200.0f;
	CloseRangeSpeed = 400.0;
	ChaseSpeed = 600.0f;

	SightRange = 1200.0f;
	AttackRange = 300.0f;
	RangedAttackRange = 1000.0;

}

void ACSRogueMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSRogueMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSRogueMonster::LookAtPlayer()
{
	if (!PlayerPawn) return;

	const FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
	const FRotator LookAtRotation = Direction.Rotation();
	const FRotator TargetRotation(0.f, LookAtRotation.Yaw, 0.f);

	SetActorRotation(TargetRotation);
}

void ACSRogueMonster::BeginAttack()
{
	if (bIsDead || bIsAttack || bIsRangeAttack) return;

	bIsAttack = true;

	LookAtPlayer();

	AAIController* AIController = Cast<AAIController>(GetController());

	if (AIController)
	{
		AIController->StopMovement();
	}
}

void ACSRogueMonster::BeginRangeAttack()
{
	if (bIsDead || bIsAttack || bIsRangeAttack) return;

	bIsRangeAttack = true;

	LookAtPlayer();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
}

void ACSRogueMonster::EndAttack()
{
	bIsAttack = false;
	bIsRangeAttack = false;
}

void ACSRogueMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{

}

void ACSRogueMonster::Die()
{
	bIsDead = true;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();

		if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AICon->BrainComponent))
		{
			BTComp->StopTree(EBTStopMode::Safe);
		}
	}

	SetLifeSpan(5.0f);
}
