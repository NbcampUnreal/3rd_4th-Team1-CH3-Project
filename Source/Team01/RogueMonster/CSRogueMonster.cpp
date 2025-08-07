#include "CSRogueMonster.h"
#include "CSRogueMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CSRogueMonsterAnimInstance.h"
#include "AIController.h"
#include "Engine/World.h"
#include "CSPlayerCharacter.h"

ACSRogueMonster::ACSRogueMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSRogueMonsterAIController::StaticClass();

	bIsDead = false;

	HitDamage = 30.0;

	MaxHP = 50.0f;

	CurrentHP = MaxHP;

}

void ACSRogueMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSRogueMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSRogueMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{

}

