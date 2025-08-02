#include "CSMonster.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSMonsterAIController.h"
#include "CSMonsterAnimInstance.h"
#include "AIController.h"

ACSMonster::ACSMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACSMonsterAIController::StaticClass();

	bIsDead = false;
	bIsAttack = false;
	bIsHit = false;
	bIsDetectedPlayer = false;

	MaxHP = 40.0f;
	CurrentHP = MaxHP;
	AttackDamage = 10.0f;

	SightRange = 2000.0f;
	AttackRange = 550.0f;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(150.f, 20.f));
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void ACSMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSMonster::OnTakeDamage);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
    	{
    		HPBar = Cast<UCS_WBP_EnemyHPBar>(Widget);
    		if (HPBar)
    		{
    			HPBar->UpdateHP(CurrentHP / MaxHP);
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

	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
	
	if (PlayerPawn)
	{
		UGameplayStatics::ApplyDamage(PlayerPawn, AttackDamage, GetController(), this, nullptr);
	}
}

void ACSMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	SetCurrentState(ECharacterState::Idle);
}

void ACSMonster::Die()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	bIsDead = true;
	bIsAttack = false;

	SetCurrentState(ECharacterState::Dead);

	if (AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->StopAllMontages(0.2f);
	}

	if (DeadMontage)
	{
		PlayAnimMontage(DeadMontage);
	}

	AIController->UnPossess();
	SetLifeSpan(5.0f);
}

void ACSMonster::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	AAIController* AIController = Cast<AAIController>(GetController());

	if (AIController)
	{
		AIController->StopMovement();
	}


	bIsHit = true;
	//if (HitMontage)
	//{
	//	PlayAnimMontage(HitMontage);
	//}
}

