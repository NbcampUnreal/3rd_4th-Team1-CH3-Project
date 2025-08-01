#include "CSBossMonster.h"
#include "CSBossAIController.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Team01/Character/CSPlayerCharacter.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "GameFramework/CharacterMovementComponent.h"


ACSBossMonster::ACSBossMonster()
{
	MaxHP = 200.0f;
	CurrentHP = MaxHP;
	AttackDamage = 20.0f;

	AIControllerClass = ACSBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(200.f, 25.f)); // 보스는 약간 더 큼직하게
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
}

void ACSBossMonster::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
}


void ACSBossMonster::BeginAttack() //공격 시작 로직
{
	// 변경: bIsNowAttacking = true; 대신 상태를 'Attacking'으로 설정합니다.
	SetCurrentState(ECharacterState::Attacking);
	UE_LOG(LogTemp, Log, TEXT("Boss has started its attack!"));
}


void ACSBossMonster::EndAttack(UAnimMontage* InMontage, bool bInterruped) //공격 종료 로직
{
	SetCurrentState(ECharacterState::Idle);


	UE_LOG(LogTemp, Log, TEXT("Boss has finished its attack. Interrupted: %s"), bInterruped ? TEXT("Yes") : TEXT("No"));
}

void ACSBossMonster::AttackHitCheck()
{
	TArray<FHitResult> OutHitResults;
	FVector Start = GetActorLocation();
	float Radius = 300.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start, Start,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResults,
		true);

	if (bIsHit)
	{
		// 이번 공격에서 이미 데미지를 입은 액터들을 기록할 배열입니다.
		TArray<AActor*> DamagedActors;

		for (const FHitResult& HitResult : OutHitResults)
		{
			if (ACSPlayerCharacter* Player = Cast<ACSPlayerCharacter>(HitResult.GetActor()))
			{
				// 이미 데미지를 입혔던 액터인지 확인하고, 입혔다면 건너뜁니다.
				if (DamagedActors.Contains(Player))
				{
					continue;
				}

				FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				float DotProduct = FVector::DotProduct(GetActorForwardVector(), DirectionToPlayer);

				if (DotProduct > 0.5f)
				{
					UGameplayStatics::ApplyDamage(
						Player,
						GetAttackDamage(),
						GetController(),
						this,
						nullptr);

					// 데미지를 입혔다고 기록합니다.
					DamagedActors.Add(Player);
				}
			}
		}
	}
}

//데미지를 받았을 때 호출될 함수
float ACSBossMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 변경: !bIsDead 대신 GetIsDead() 함수 또는 상태 직접 비교로 변경합니다.
	if (FinalDamage > 0.f && GetCurrentState() != ECharacterState::Dead)
	{
		CurrentHP -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Boss took %f damage, Current Health: %f"), FinalDamage, CurrentHP);

		if (CurrentHP <= 0.f)
		{
			Die();
		}
		else
		{
			// 공격 중일 때는 피격 애니메이션을 재생하지 않도록 '슈퍼아머'처럼 만듭니다.
			if (GetCurrentState() != ECharacterState::Attacking)
			{
				SetCurrentState(ECharacterState::HitReaction);
				GetCharacterMovement()->StopMovementImmediately();

				ACSBossAIController* AIController = Cast<ACSBossAIController>(GetController());
				if (AIController)
				{
					AIController->StopMovement();
				}


				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance && HitReactMontage)
				{
					AnimInstance->Montage_Play(HitReactMontage);

					// 몽타주가 끝나면 다시 Idle 상태로 돌아오도록 설정합니다.
					FOnMontageEnded MontageEndedDelegate;
					MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted) {
						SetCurrentState(ECharacterState::Idle);
						});
					AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HitReactMontage);
				}
			}
		}
	}

	return FinalDamage;
}

//사망 처리 함수
void ACSBossMonster::Die()
{
	if (GetCurrentState() == ECharacterState::Dead) return;

	SetCurrentState(ECharacterState::Dead);

	// 캡슐 컴포넌트의 충돌을 꺼서 죽은 뒤에 시체가 다른 액터를 막지 않도록 합니다.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AI 로직을 정지시킵니다.
	ACSBossAIController* AIController = Cast<ACSBossAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Boss is Dead"));
	}

	UE_LOG(LogTemp, Error, TEXT("Boss is Dead!"));

	GetWorld()->GetTimerManager().SetTimer(
		DeathTimerHandle,
		this,
		&ACSBossMonster::GoRagdoll,
		0.77f,
		false);
}

void ACSBossMonster::GoRagdoll()
{
	if (GetMesh())
	{
		// 스켈레탈 메시의 콜리전 프로파일을 'Ragdoll'로 변경하고,
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		// 물리 시뮬레이션을 활성화합니다.
		GetMesh()->SetSimulatePhysics(true);

		GetWorld()->GetTimerManager().SetTimer(
			DisappearTimerHandle,
			this,
			&ACSBossMonster::Disappear,
			3.0f,
			false);
	}
}

void ACSBossMonster::Disappear()
{
	Destroy();
}