#include "CSMonster.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Character/Controller/CSPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSMonsterAIController.h"
#include "CSMonsterAnimInstance.h"
#include "AIController.h"
#include "CSFireBomb.h"

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

	SightRange = 2000.0f;
	AttackRange = 550.0f;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(150.f, 20.f));
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HPBarComponent->SetVisibility(false);
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

	ThrowActor();
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

	AIController->UnPossess();

	if (AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->StopAllMontages(0.2f);
	}

	AIController->UnPossess();

	if (LastInstigator)
	{
		if (ACSPlayerController* PlayerController = Cast<ACSPlayerController>(LastInstigator))
		{
			if (UCS_WBP_HUD* HUD = PlayerController->GetHUDWidget())
			{
				PlayerController->AddKillCount();
				HUD->AddKillLogEntry(TEXT("Player"), GetName(), nullptr);
				HUD->ShowKillConfirmMessage(TEXT("Kill!!"));
			}
		}
	}
	
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

	CurrentHP -= Damage;

	if (HPBarComponent)
	{
		HPBarComponent->SetVisibility(true);
		
		GetWorld()->GetTimerManager().ClearTimer(HPHideTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(HPHideTimerHandle, [this]()
		{
			if (HPBarComponent)
			{
				HPBarComponent->SetVisibility(false);
			}
		}, 3.0f, false);
	}
	
	if (HPBar)
	{
		HPBar->UpdateHP(CurrentHP / MaxHP);
	}
	
	LastInstigator = InstigatedBy;
	
	if (CurrentHP <= 0.0f && !bIsDead)
	{
		Die();
	}

}

void ACSMonster::ThrowActor()
{
	if (!ThrownActorClass) return;

	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, 50.f);
	FRotator SpawnRot = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (auto* ThrownActor = GetWorld()->SpawnActor<ACSFireBomb>(ThrownActorClass, SpawnLoc, SpawnRot, SpawnParams))
	{
		FVector ThrowDir = GetActorForwardVector() + FVector(0, 0.0f, 0.3f);
		ThrowDir.Normalize();

		ThrownActor->LaunchProjectile(ThrowDir, 1200.f);
	}
}