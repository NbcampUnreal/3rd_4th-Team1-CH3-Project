#include "CSMummyMonster.h"
#include "CSMummyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSMummyMonsterAnimInstance.h"
#include "AIController.h"


ACSMummyMonster::ACSMummyMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSMummyAIController::StaticClass();

	bIsDead = false;

	bIsAttack = false;

	bIsDetectedPlayer = false;

	bIsDead = false;

	AttackRange = 200.0f;

	SightRange = 800.0f;

}

void ACSMummyMonster::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &ACSMummyMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSMummyMonster::BeginAttack()
{
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

}

void ACSMummyMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{

}

void ACSMummyMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (CurrentHP <= 0.0f)
	{
		bIsDead = true;

		//Die();
	}
}
