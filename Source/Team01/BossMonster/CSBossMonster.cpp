#include "CSBossMonster.h"
#include "CSBossAIController.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	bIsInPhase2 = false;
	bIsPhaseTransitionPending = false;

	AIControllerClass = ACSBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(RootComponent);
	HPBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarComponent->SetDrawSize(FVector2D(200.f, 25.f)); // 보스는 약간 더 큼직하게
	HPBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	HPBarComponent->SetVisibility(false);
}

void ACSBossMonster::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
	{
		HPBar = Cast<UCS_WBP_EnemyHPBar>(Widget);
	}

	DefaultGravityScale = GetCharacterMovement()->GravityScale;

	
}

void ACSBossMonster::BeginAttackPattern(EBossAttackType AttackType)
{
	//공격을 실행할 수 있는지 가장 먼저 확인
	if (!AttackMontages.Contains(AttackType))
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontages TMap does not contain AttackType: %d"), AttackType);
		return;
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	SetCurrentState(ECharacterState::Attacking);

	// 공격 타입에 맞는 특별 로직을 안전하게 실행합니다.
	if (AttackType == EBossAttackType::GroundSlam)
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AActor* TargetPlayer = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetPlayer")));
			if (TargetPlayer)
			{
				// AI의 현재 이동/경로 탐색을 즉시 중단시켜 '유령 브레이크'를 해제합니다.
				/*AIController->StopMovement();*/

				/*if (AIController->GetBrainComponent())
				{
					AIController->GetBrainComponent()->PauseLogic(TEXT("Jumping Attack"));
				}*/

				// 플레이어를 향해 방향을 회전합니다.
				FVector DirectionToPlayer = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				SetActorRotation(FRotator(0.f, DirectionToPlayer.Rotation().Yaw, 0.f));

				// 계산된 힘으로 캐릭터를 점프시킵니다.
				FVector Temp = DirectionToPlayer * GroundSlamJumpXYPower;
				FVector LaunchVelocity = (DirectionToPlayer * GroundSlamJumpXYPower) + FVector(0, 0, GroundSlamJumpZPower);

				/*this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);*/

				LaunchCharacter(LaunchVelocity, true, true);

				GetCharacterMovement()->GravityScale = GroundSlamGravityScale;

				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Temp 변수 내용: %s"), *Temp.ToString()));
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("LaunchVelocity: %s"), *LaunchVelocity.ToString()));
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("DirectionToPlayer: %s"), *DirectionToPlayer.ToString()));
			}
		}
	}

	// 4. 마지막으로 애니메이션 몽타주를 재생하고, 공격이 끝났을 때의 처리를 예약합니다.
	TObjectPtr<UAnimMontage> MontageToPlay = AttackMontages[AttackType];
	AnimInstance->Montage_Play(MontageToPlay);

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &ACSBossMonster::EndAttack);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
}

void ACSBossMonster::EndAttack(UAnimMontage* InMontage, bool bInterrupted)
{
	Super::EndAttack(InMontage, bInterrupted);

	/*AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->ResumeLogic(TEXT("Attack Finished"));
	}*/

	// 공격이 중단되지 않고 정상적으로 끝났을 때
	if (!bInterrupted)
	{
		SetCurrentState(ECharacterState::Idle); // 상태를 Idle로 변경
		TryStateTransition(); // 여기서 2페이즈 전환 요청이 있었는지 체크
	}
}

void ACSBossMonster::ApplyMeleeDamage()
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

void ACSBossMonster::ApplyGroundSlamDamage()
{
	// 공격의 중심점은 보스의 정면보다 150cm 앞
	const FVector ForwardVector = GetActorForwardVector();
	const FVector DamageOrigin = GetActorLocation() + (ForwardVector * GroundSlamForwardOffset);

	// 무시할 액터 목록
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	// 강력한 범위 공격을 실행합니다!
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),             // 현재 월드
		GroundSlamDamage,       // 위에서 설정한 대미지 값
		DamageOrigin,           // 공격의 중심점
		GroundSlamRadius,       // 공격의 반경
		nullptr,                // 대미지 타입 (기본값으로 둡니다)
		IgnoreActors,           // 이 공격에 맞지 않을 액터 목록
		this,                   // 공격을 가한 장본인 (보스 자신)
		GetController(),        // 공격을 가한 컨트롤러
		true                    // 벽과 같은 장애물을 무시하고 대미지를 줄지 여부
	);

	// 공격 타이밍에 맞춰 이펙트와 사운드를 재생
	if (GroundSlamVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GroundSlamVFX, DamageOrigin);
	}
	if (GroundSlamSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GroundSlamSFX, DamageOrigin);
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
		
		if (HPBarComponent)
		{
			HPBarComponent->SetVisibility(true);
			
			GetWorld()->GetTimerManager().ClearTimer(HPHideTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(HPHideTimerHandle, [this]()
			{
				if (HPBarComponent && GetCurrentState() != ECharacterState::Dead)
				{
					HPBarComponent->SetVisibility(false);
				}
			}, 3.0f, false);
		}
		
		if (HPBar)
		{
			HPBar->UpdateHP(CurrentHP / MaxHP);
		}
		
		if (!bIsInPhase2 && (CurrentHP / MaxHP <= 0.5f))
		{
			bIsPhaseTransitionPending = true; // 2페이즈 진입 플래그
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
						if (!bInterrupted)
						{
							SetCurrentState(ECharacterState::Idle);
						}
						});
					AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HitReactMontage);
				}
			}
		}
	}
	TryStateTransition();
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
			if (!bInterrupted)
			{
				SetCurrentState(ECharacterState::Idle);
			}
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

void ACSBossMonster::TryStateTransition() // 하고 있던 일이 끝나는 시점과 데미지를 입은 직후에 상태전환을 체크할 함수
{
	// 2페이즈 전환이 요청되었고, 현재 캐릭터가 다른 중요 행동(공격 등)을 하고 있지 않을 때
	if (bIsPhaseTransitionPending && GetCurrentState() != ECharacterState::Attacking && GetCurrentState() != ECharacterState::Shouting)
	{
		bIsPhaseTransitionPending = false;
		EnterPhase2();
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

void ACSBossMonster::OnLanded(const FHitResult& Hit)
{
	// 캐릭터가 땅에 착지하면, 중력 배율을 원래대로 되돌립니다.
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	UE_LOG(LogTemp, Warning, TEXT("Boss Landed! GravityScale Restored."));
}