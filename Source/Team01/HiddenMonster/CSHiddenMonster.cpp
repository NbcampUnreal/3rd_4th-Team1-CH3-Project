#include "CSHiddenMonster.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Character/Controller/CSPlayerController.h"
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

	MaxHP = 10.0f;
	CurrentHP = MaxHP;

	AttackRange = 200.0f;
	AttackDamage = 20.0f;

	bIsAttack = false;
	bIsDead = false;
	bIsDetected = false;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(150.f, 20.f));
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HPBarComponent->SetVisibility(false);

	ScoreValue = 200;
}

void ACSHiddenMonster::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ACSHiddenMonster::OnTakeDamage);

	SightCone->OnComponentBeginOverlap.AddDynamic(this, &ACSHiddenMonster::OnConeOverlap);

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

float ACSHiddenMonster::GetCurrentHP()
{
	return CurrentHP;
}

void ACSHiddenMonster::OnConeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		APawn* Player = Cast<APawn>(OtherActor);

		if (!Player || !Player->IsPlayerControlled())
		{
			return;
		}

		bIsDetected = true;

		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
			{
				BB->SetValueAsObject(TEXT("TargetActor"), Player);
			}
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
	
	ShowFloatingDamage(FMath::RoundToInt(Damage));
	
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
		const float Ratio = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 0.f;
		HPBar->UpdateHP(Ratio);
	}
	
	LastInstigator = InstigatedBy;
	
	if (CurrentHP <= 0)
	{
		bIsDead = true;

		Die();
	}
}