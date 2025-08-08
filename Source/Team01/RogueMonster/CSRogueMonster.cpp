#include "CSRogueMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSRogueMonsterAIController.h"
#include "CSRogueMonsterAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Character/Controller/CSPlayerController.h"
#include "CSDagger.h"

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
	bIsOverlap = false;

	MaxHP = 150.0f;
	CurrentHP = MaxHP;
	HitDamage = 20.0f;
	RangedHitDamage = 30.0f;

	PatrolSpeed = 200.0f;
	CloseRangeSpeed = 400.0;
	ChaseSpeed = 600.0f;

	SightRange = 1200.0f;
	AttackRange = 300.0f;
	RangedAttackRange = 1000.0;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(150.f, 20.f));
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	bAlwaysShowHPBar = true;

	ScoreValue = 100;

}

void ACSRogueMonster::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (HPBarComponent)
	{
		HPBarComponent->SetVisibility(bAlwaysShowHPBar);

		if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
		{
			HPBar = Cast<UCS_WBP_EnemyHPBar>(Widget);
			if (HPBar)
			{
				const float Ratio = FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
				HPBar->SetHP(CurrentHP, MaxHP);
			}
		}
	}
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

void ACSRogueMonster::ApplyAttackDamage()
{
	if (bIsDead || !PlayerPawn) return;

	const float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance > AttackRange) return;

	UGameplayStatics::ApplyDamage(
		PlayerPawn,
		HitDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
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

	if (DaggerClass && PlayerPawn)
	{
		FVector MuzzleLocation = GetActorLocation() + FVector(0, 0, 50) + GetActorForwardVector() * 100.f;
		FVector Direction = (PlayerPawn->GetActorLocation() - MuzzleLocation).GetSafeNormal();

		FActorSpawnParameters SpawnParams;
		ACSDagger* Dagger = GetWorld()->SpawnActor<ACSDagger>(DaggerClass, MuzzleLocation, FRotator::ZeroRotator, SpawnParams);
		if (Dagger)
		{
			Dagger->SetDirection(Direction);
		}
	}
}

void ACSRogueMonster::EndAttack()
{
	bIsAttack = false;
	bIsRangeAttack = false;
}

float ACSRogueMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsOverlap)
	{
		return 0.f;
	}

	CurrentHP -= DamageAmount;

	ShowFloatingDamage(FMath::RoundToInt(DamageAmount));
	
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
		HPBar->SetHP(CurrentHP, MaxHP);
	}
	
	LastInstigator = EventInstigator;
	
	if (CurrentHP <= 0.f)
	{
		Die();
	}

	return DamageAmount;
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

	if (LastInstigator)
	{
		if (ACSPlayerController* PlayerController = Cast<ACSPlayerController>(LastInstigator))
		{
			if (UCS_WBP_HUD* HUD = PlayerController->GetHUDWidget())
			{
				PlayerController->AddKillCount();
				PlayerController->AddScore(ScoreValue);
				HUD->AddKillLogEntry(TEXT("Player"), GetName(), nullptr);
				HUD->ShowKillConfirmMessage(TEXT("Kill!!"));
			}
		}
	}

	SetLifeSpan(5.0f);
}

void ACSRogueMonster::ResetOverlap()
{
	bIsOverlap = false;
}

void ACSRogueMonster::ShowFloatingDamage(int32 Value)
{
	// 간단한 디버그 로그 버전
	UE_LOG(LogTemp, Warning, TEXT("[FloatingDamage] %d"), Value);
}