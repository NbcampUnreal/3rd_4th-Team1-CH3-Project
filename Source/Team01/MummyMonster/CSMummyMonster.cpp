#include "CSMummyMonster.h"
#include "CSMummyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CSMummyMonsterAnimInstance.h"
#include "AIController.h"
#include "Engine/World.h"
#include "CSPlayerCharacter.h"


ACSMummyMonster::ACSMummyMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSMummyAIController::StaticClass();

	bIsDead = false;

	HitDamage = 30.0;

	MaxHP = 1.0f;

	CurrentHP = MaxHP;

}

void ACSMummyMonster::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &ACSMummyMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACSMummyMonster::OnPlayerOverlap);

}

void ACSMummyMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHP -= Damage;
	if (bIsDead) return;

	if (CurrentHP <= 0.0f)
	{
		bIsDead = true;

		Die();
	}
}

void ACSMummyMonster::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;


	if (Cast<ACSPlayerCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			HitDamage,
			GetController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

void ACSMummyMonster::Die()
{
	if (bIsDead == false) return;

	UWorld* World = GetWorld();
	if (World)
	{
		for (int32 i = 0; i < 2; i++)
		{
			FVector SpawnLocation = GetActorLocation() + FVector(FMath::RandRange(-200.f, 200.f), FMath::RandRange(-200.f, 200.f), 0.f);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			ACSMummyMonster* Spawned = World->SpawnActor<ACSMummyMonster>(GetClass(), SpawnLocation, SpawnRotation);

			if (Spawned)
			{
				Spawned->PatrolPoints = PatrolPoints;
			}
		}
	}

	Destroy();
}
