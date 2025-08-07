#include "CSRogueMonster.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Character/Controller/CSPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSRogueMonsterAIController.h"
#include "CSRogueMonsterAnimInstance.h"
#include "AIController.h"

ACSRogueMonster::ACSRogueMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSRogueMonsterAIController::StaticClass();

	bIsDead = false;
	bIsAttack = false;
	bIsHit = false;
	bIsDetectedPlayer = false;

	MaxHP = 40.0f;
	CurrentHP = MaxHP;

	SightRange = 1000.0f;
	AttackRange = 200.0f;

}

void ACSRogueMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSRogueMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSRogueMonster::BeginAttack()
{
	bIsAttack = true;

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

void ACSRogueMonster::EndAttack()
{
	bIsAttack = false;
}

void ACSRogueMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{

}
