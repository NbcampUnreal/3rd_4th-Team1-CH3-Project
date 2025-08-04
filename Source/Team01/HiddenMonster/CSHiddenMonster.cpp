#include "CSHiddenMonster.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CSHiddenMonsterAIController.h"

ACSHiddenMonster::ACSHiddenMonster()
{
	SightCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SightCone"));
	SightCone->SetupAttachment(RootComponent);

	SightCone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SightCone->SetCollisionResponseToAllChannels(ECR_Ignore);
	SightCone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SightCone->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->MaxAcceleration = 100000.f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSHiddenMonsterAIController::StaticClass();

	MaxHP = 20.0f;
	CurrentHP = MaxHP;

	AttackRange = 200.0f;
	AttackDamage = 20.0f;

	bIsAttack = false;
	bIsDead = false;
	bIsDetected = false;
}

void ACSHiddenMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSHiddenMonster::OnTakeDamage);

	SightCone->OnComponentBeginOverlap.AddDynamic(this, &ACSHiddenMonster::OnConeOverlap);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ACSHiddenMonster::BeginAttack()
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

	if (PlayerPawn)
	{
		float DamageAmount = AttackDamage;
		UGameplayStatics::ApplyDamage(PlayerPawn, DamageAmount, GetController(), this, UDamageType::StaticClass());
	}
}

void ACSHiddenMonster::AttackEnd()
{
	bIsAttack = false;
}

void ACSHiddenMonster::Die()
{
	SetLifeSpan(5.0f);
}

float ACSHiddenMonster::GetCurrentHP()
{
	return CurrentHP;
}

void ACSHiddenMonster::OnConeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	bIsDetected = true;

	if (OtherActor && OtherActor != this)
	{
		APawn* Player = Cast<APawn>(OtherActor);
		if (!Player) return;

		AAIController* AICon = Cast<AAIController>(GetController());
		if (!AICon) return;

		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsObject(TEXT("TargetActor"), Player);
		}
	}
}

void ACSHiddenMonster::OnConeEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	if (!OtherActor || OtherActor == this) return;

	APawn* Player = Cast<APawn>(OtherActor);
	if (!Player) return;

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
		}
	}
}


void ACSHiddenMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHP -= Damage;

	if (CurrentHP <= 0)
	{
		bIsDead = true;

		Die();
	}
}