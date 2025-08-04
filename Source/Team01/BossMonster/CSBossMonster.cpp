#include "CSBossMonster.h"
#include "CSBossAIController.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "Animation/AnimInstance.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Team01/Character/CSPlayerCharacter.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Character/Controller/CSPlayerController.h"
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

	if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
	{
		HPBar = Cast<UCS_WBP_EnemyHPBar>(Widget);
	}
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
	if (GetCurrentState() == ECharacterState::Shouting || GetCurrentState() == ECharacterState::PhaseTransition)
	{
		return 0.0f;
	}

	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	LastInstigator = EventInstigator;
	
	if (FinalDamage > 0.f && GetCurrentState() != ECharacterState::Dead)
	{
		CurrentHP -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Boss took %f damage, Current Health: %f"), FinalDamage, CurrentHP);

		if (HPBar)
		{
			HPBar->UpdateHP(CurrentHP / MaxHP);
		}
		
		if (!bIsInPhase2 && (CurrentHP / MaxHP <= 0.5f))
		{
			EnterPhase2(); // 2페이즈 진입
		}

		if (CurrentHP <= 0.f)
		{
			Die();
		}
		else
		{
			// 공격 중일 때는 피격 애니메이션을 재생하지 않도록 슈퍼아머 상태
			if (GetCurrentState() != ECharacterState::Attacking && !bIsInPhase2) // 페이즈 2가 아닐때 라는 조건 추가
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

void ACSBossMonster::EnterPhase2()
{
	if (bIsInPhase2) return; // 이미 2페이즈라면 중복 실행 방지

	bIsInPhase2 = true;
	SetCurrentState(ECharacterState::PhaseTransition);
	UE_LOG(LogTemp, Error, TEXT("Boss is entering PHASE 2!"));

	// 2페이즈  몽타주를 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Phase2TransitionMontage)
	{
		AnimInstance->Montage_Play(Phase2TransitionMontage);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted) {
			SetCurrentState(ECharacterState::Idle);
			});
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Phase2TransitionMontage);
	}

	//  2페이즈 전환 파티클(VFX)을 재생
	if (Phase2TransitionVFX)
	{
		// 캐릭터의 발밑(루트 본)에 붙여서 파티클을 생성
		UGameplayStatics::SpawnEmitterAttached(
			Phase2TransitionVFX,
			GetMesh(),
			TEXT("FX_Root"), // 소켓 이름 지정
			FVector(EForceInit::ForceInit),
			FRotator(EForceInit::ForceInit),
			EAttachLocation::SnapToTarget
		);
	}
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

	if (LastInstigator)
	{
		if (ACSPlayerController* PlayerController = Cast<ACSPlayerController>(LastInstigator))
		{
			if (UCS_WBP_HUD* HUD = PlayerController->GetHUDWidget())
			{
				PlayerController->AddKillCount();
				HUD->AddKillLogEntry(TEXT("Player"), GetName(), nullptr);
				HUD->ShowKillConfirmMessage(TEXT("Boss Kill!!"));
			}
		}
	}
	
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