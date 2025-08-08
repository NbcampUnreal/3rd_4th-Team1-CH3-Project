#include "CSBossMonster.h"
#include "CSBossAIController.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "../Game/CSGameStateBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Team01/Character/CSPlayerCharacter.h"
#include "../UI/CS_WBP_EnemyHPBar.h"
#include "../Ui/CS_WBP_HUD.h"
#include "../Ui/CSGameClear.h"
#include "../Character/Controller/CSPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"


ACSBossMonster::ACSBossMonster()
{
	MaxHP = 300.0f;
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

	ScoreValue = 500;
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

	// 스켈레탈 메시에 적용된 머티리얼의 개수만큼
	const int32 MaterialCount = GetMesh()->GetNumMaterials();
	for (int32 i = 0; i < MaterialCount; ++i)
	{
		// 원본 머티리얼을 가져옵니다.
		UMaterialInterface* OriginalMaterial = GetMesh()->GetMaterial(i);
		if (OriginalMaterial)
		{
			// 원본을 기반으로 동적 머티리얼 인스턴스를 생성
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			// 생성된 동적 머티리얼을 다시 메시에 적용
			GetMesh()->SetMaterial(i, DynamicMaterial);
			// 나중에 제어하기 쉽도록 배열에 추가
			BossDynamicMaterials.Add(DynamicMaterial);
		}
	}

	OriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACSBossMonster::OnHit);
}

void ACSBossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 2페이즈 전환 상태일 때만 플레이어를 바라보도록 처리
	if (GetCurrentState() == ECharacterState::PhaseTransition)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			// 보스에서 플레이어를 바라보는 방향 계산
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());

			// Z축(Yaw) 회전만 부드럽게 적용
			FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 2.0f));
		}
	}

	if (bIsTransitioningMaterial)
	{
		// 현재 값을 목표 값으로 부드럽게하는 보간작업
		CurrentGlowAmount = FMath::FInterpTo(CurrentGlowAmount, TargetGlowAmount, DeltaTime, MaterialTransitionSpeed);
		CurrentEnrageValue = FMath::FInterpTo(CurrentEnrageValue, TargetEnrageValue, DeltaTime, MaterialTransitionSpeed);

		// --- Element 0번부터 3번 머티리얼의 "GlowAmount" 파라미터 변경 ---
		for (int32 i = 0; i <= 3; ++i)
		{
			// 해당 인덱스의 머티리얼이 유효한지 확인
			if (BossDynamicMaterials.IsValidIndex(i) && BossDynamicMaterials[i])
			{
				BossDynamicMaterials[i]->SetScalarParameterValue(FName("GlowAmount"), CurrentGlowAmount);
			}
		}

		// --- Element 7번 머티리얼의 "Enrage" 파라미터 변경 ---
		if (BossDynamicMaterials.IsValidIndex(7) && BossDynamicMaterials[7]) // 7번 인덱스 유효성 확인
		{
			BossDynamicMaterials[7]->SetScalarParameterValue(FName("Enrage"), CurrentEnrageValue);
		}

		// 전환이 거의 완료되었는지 확인
		if (FMath::IsNearlyEqual(CurrentGlowAmount, TargetGlowAmount, 0.01f) &&
			FMath::IsNearlyEqual(CurrentEnrageValue, TargetEnrageValue, 0.01f))
		{
			bIsTransitioningMaterial = false; // 불필요한 연산을 막기 위해 전환 상태를 종료
		}
	}

	if (GetCurrentState() == ECharacterState::Charging)
	{
		// 설정된 방향(ChargeDirection)으로 계속 전진합니다.
		AddMovementInput(ChargeDirection, 1.0f);
		// 캐릭터 이동 속도를 돌격 속도로 강제 설정합니다.
		GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
	}


}

void ACSBossMonster::BeginAttackPattern(EBossAttackType AttackType)
{
	// 공격 타입에 해당하는 'FAttackMontageArray' 구조체를 찾기
	const FAttackMontageArray* MontageStruct = AttackMontages.Find(AttackType);

	// 구조체가 유효하지 않거나, 구조체 안의 몽타주 배열이 비어있으면 함수를 종료
	if (!MontageStruct || MontageStruct->Montages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontages TMap does not contain valid montages for AttackType: %d"), AttackType);
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	SetCurrentState(ECharacterState::Attacking);

	// 구조체 안의 Montages 배열에서 랜덤 인덱스를 선택합니다.
	const int32 RandomIndex = FMath::RandRange(0, MontageStruct->Montages.Num() - 1);
	// 랜덤 인덱스에 해당하는 몽타주를 가져옵니다.
	TObjectPtr<UAnimMontage> MontageToPlay = MontageStruct->Montages[RandomIndex];

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

	if (MontageToPlay)
	{
		AnimInstance->Montage_Play(MontageToPlay);

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ACSBossMonster::EndAttack);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
	}
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

void ACSBossMonster::BeginChargeAttack()
{
	// Idle 상태가 아닐 땐 공격을 시작하지 못하게 막습니다.
	if (GetCurrentState() != ECharacterState::Idle) return;

	DamagedActorsInCharge.Empty();

	// 1. 공격 상태로 전환하고 변수 초기화
	SetCurrentState(ECharacterState::Charging);

	// 2. 플레이어 방향으로 초기 돌격 방향 설정
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		ChargeDirection = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ChargeDirection.Z = 0; // 수평으로만 움직이도록 Z값 제거
	}

	// 3. ⭐ AI 두뇌(BT)를 잠시 멈춰서 다른 행동을 못하게 막습니다.
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->PauseLogic(TEXT("Start Charging Attack"));
	}

	// 4. 구르기 애니메이션 몽타주 재생
	const FAttackMontageArray* MontageStruct = AttackMontages.Find(EBossAttackType::RollAndCharge);
	if (MontageStruct && MontageStruct->Montages.Num() > 0)
	{
		if (UAnimMontage* ChargeMontage = MontageStruct->Montages[0])
		{
			GetMesh()->GetAnimInstance()->Montage_Play(ChargeMontage);
		}
	}

	// 5. 돌격하는 동안에는 캐릭터가 자동으로 방향을 바꾸지 않도록 합니다.
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorRotation(ChargeDirection.Rotation()); // 초기 방향으로 몸을 돌려줍니다.
}

void ACSBossMonster::EndChargeAttack()
{
	// 1. 상태를 다시 평시(Idle)로 되돌립니다.
	SetCurrentState(ECharacterState::Idle);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f); // 모든 애니메이션 중지

	// 2. ⭐ 멈췄던 AI 두뇌(BT)를 다시 깨웁니다.
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->ResumeLogic(TEXT("End Charging Attack"));
	}

	// 3. 캐릭터 이동 설정을 원래대로 되돌립니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
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
		// 1. 데미지를 준 대상이 '플레이어'인지 확인합니다.
		if (DamageCauser && DamageCauser->IsA(ACSPlayerCharacter::StaticClass()))
		{
			// 2. AI 컨트롤러와 블랙보드를 가져옵니다.
			if (AAIController* AICon = Cast<AAIController>(GetController()))
			{
				if (UBlackboardComponent* BlackboardComp = AICon->GetBlackboardComponent())
				{
					// 3. 블랙보드의 bHasBeenAlerted 키를 true로 강제로 바꿔버립니다.
					BlackboardComp->SetValueAsBool(TEXT("bHasBeenAlerted"), true);
					UE_LOG(LogTemp, Warning, TEXT("Boss has been alerted by taking damage!"));
				}
			}
		}

		CurrentHP -= FinalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Boss took %f damage, Current Health: %f"), FinalDamage, CurrentHP);

		ShowFloatingDamage(FMath::RoundToInt(FinalDamage));

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

	// 머티리얼 전환을 시작하라는 플래그
	bIsTransitioningMaterial = true;
	TargetGlowAmount = 300.0f; // GlowAmount의 최종 목표치
	TargetEnrageValue = 2.0f;     // Enrage의 최종 목표치
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

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ACSPlayerController* PlayerController = Cast<ACSPlayerController>(PC))
		{
			// 킬로그 및 점수 처리
			if (UCS_WBP_HUD* HUD = PlayerController->GetHUDWidget())
			{
				PlayerController->AddKillCount();
				PlayerController->AddScore(ScoreValue);
				HUD->AddKillLogEntry(TEXT("Player"), GetName(), nullptr);
				HUD->ShowKillConfirmMessage(TEXT("Boss Kill!!"));
			}
		}
	}

	if (ACSGameStateBase* GS = GetWorld()->GetGameState<ACSGameStateBase>())
	{
		GS->SetMissionState(EMissionState::MissionClear);
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

void ACSBossMonster::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 돌격 상태가 아니거나, 부딪힌 대상이 유효하지 않거나, 자기 자신이면 무시합니다.
	if (GetCurrentState() != ECharacterState::Charging || !OtherActor || OtherActor == this)
	{
		return;
	}

	// 1. 플레이어와 부딪혔는지 먼저 확인합니다.
	if (OtherActor->IsA(ACSPlayerCharacter::StaticClass()))
	{
		// 이번 돌진에서 아직 데미지를 주지 않은 플레이어인지 확인합니다.
		if (!DamagedActorsInCharge.Contains(OtherActor))
		{
			// 데미지를 적용합니다.
			UGameplayStatics::ApplyDamage(
				OtherActor,
				GetAttackDamage(),
				GetController(),
				this,
				nullptr
			);

			// "이미 공격한 대상" 목록에 플레이어를 추가하여 중복 데미지를 방지합니다.
			DamagedActorsInCharge.Add(OtherActor);
		}
		// 플레이어와 부딪혔을 때는 아무것도 하지 않고 그냥 '통과'합니다.
		// 공격은 멈추지 않고 계속됩니다.
	}
	// 2. 부딪힌 대상이 '플레이어가 아닌 경우' (벽, 장애물 등)
	else
	{
		GetCharacterMovement()->StopMovementImmediately();

		//충돌 지점의 법선(Normal) 벡터를 이용해 벽이 밀어내는 방향을 구합
		FVector StepBackDirection = Hit.ImpactNormal;

		//원하는 거리만큼 캐릭터를 강제로 뒤로 이동시킵니다.
		float StepBackDistance = 50.0f; // 50cm 정도 뒤로 물러나게 설정
		AddActorWorldOffset(StepBackDirection * StepBackDistance, true);

		BeginStun();
	}
}

void ACSBossMonster::BeginStun()
{
	// 상태를 '스턴'으로 변경
	SetCurrentState(ECharacterState::Stunned);
	// 돌진 애니메이션을 멈춥니다.
	GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);

	// 벽 충돌 사운드를 재생합니다.
	if (WallHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WallHitSound, GetActorLocation());
	}

	// 3. 스턴 애니메이션을 재생하고, 길이를 가져옵니다.
	float StunDuration = 1.5f; // 기본 스턴 시간 
	if (StunMontage)
	{
		StunDuration = GetMesh()->GetAnimInstance()->Montage_Play(StunMontage);
	}

	// 4. ⭐ 스턴 애니메이션이 끝나는 시간에 맞춰 EndChargeAttack() 함수를 호출하도록 타이머를 설정합니다.
	GetWorld()->GetTimerManager().SetTimer(
		StunEndTimerHandle,     // 타이머 핸들
		this,                   // 타이머를 소유할 액터
		&ACSBossMonster::EndChargeAttack, // 시간이 다 되면 호출할 함수
		StunDuration,           // 지연 시간 (스턴 애니메이션 길이)
		false                   // 반복하지 않음
	);
}