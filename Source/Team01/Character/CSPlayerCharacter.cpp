#include "CSPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/CSInputConfig.h"
#include "Animation/CSPlayerAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "../Team01.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ItemInterface.h"
#include "../Character/Controller/CSPlayerController.h"
#include "../Ui/CS_WBP_HUD.h"
#include "Animation/CSUltProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialExpressionBlendMaterialAttributes.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


ACSPlayerCharacter::ACSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region Character Setup
	
	float CharacterHalfHeight = 110.f;
	float CharacterRadius = 40.f;
	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GravityScale = 1.3f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;

	// 앉기 시 변화
	GetCharacterMovement()->SetCrouchedHalfHeight(55.f);
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;

	// FIKGoalData 의 Alpha 기본값, 1.f == IK Goal 적용강도 100%
	LeftHandIKGoal.Alpha = 1.f;

#pragma endregion

#pragma region Character Vision

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 450.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 이동방향으로 몸체 자동회전
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;		// 카메라가 컨트롤러 회전을 따름
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = true;

	// 배그 느낌의 살짝 우상단 시점
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 50.f, 100.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	
#pragma endregion

#pragma region Character Status

	MaxHP = 100.f;
	CurrentHP = MaxHP;
	AttackDamage = 10.f;
	
	MaxBullet = 6;
	Bullet = MaxBullet;

	Tags.Add("Player");
	
#pragma endregion
}

void ACSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem))
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}
}

void ACSPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LeftHandIKGoal.GoalTransform = CalculateLeftHandIKGoalTransform();

	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 10.f);
	CameraComponent->SetFieldOfView(CurrentFOV);

	if (bIsNowRagdollBlending)
	{
		CurrentRagdollBlendWeight =
			FMath::FInterpTo(CurrentRagdollBlendWeight, TargetRagdollBlendWeight, DeltaSeconds, 10.f);

		FName PivotBoneName = FName(TEXT("pelvis"));
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagdollBlendWeight);

		if (CurrentRagdollBlendWeight - TargetRagdollBlendWeight < KINDA_SMALL_NUMBER)
		{
			GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
			bIsNowRagdollBlending = false;
		}
	}
}

void ACSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Move,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::InputMove
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Look,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::InputLook
		);

		// 점프는 별도 구현 없이 ACharacter 함수 사용
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Jump,
			ETriggerEvent::Triggered,
			this,
			&ACharacter::Jump
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Jump,
			ETriggerEvent::Completed,
			this,
			&ACharacter::StopJumping
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Sprint,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::StartSprint
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Sprint,
			ETriggerEvent::Completed,
			this,
			&ThisClass::StopSprint
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Crouch,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::StartCrouch
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Crouch,
			ETriggerEvent::Completed,
			this,
			&ThisClass::StopCrouch
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Shoot,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::InputShoot
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Reload,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::InputReload
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->IronSight,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::StartIronSight
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->IronSight,
			ETriggerEvent::Completed,
			this,
			&ThisClass::StopIronSight
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Interact,
			ETriggerEvent::Started,
			this,
			&ACSPlayerCharacter::TryActivateNearbyItem
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Ult,
			ETriggerEvent::Triggered,
			this,
			&ThisClass::OnUltCastPressed
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Ult,
			ETriggerEvent::Completed,
			this,
			&ThisClass::OnUltCastReleased
		);
	}
}

void ACSPlayerCharacter::RecoverHealth(float Amount)
{
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.f, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("+HP: %.0f / %.0f"), CurrentHP, MaxHP);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ACSPlayerController* CSController = Cast<ACSPlayerController>(PC))
		{
			if (UCS_WBP_HUD* HUD = CSController->GetHUDWidget()) // HUD 접근
			{
				HUD->UpdateHP(CurrentHP / MaxHP); // 비율로 갱신
				UE_LOG(LogTemp, Warning, TEXT("[HUD] 체력바 %.2f%%로 갱신됨"), (CurrentHP / MaxHP) * 100.f);
			}
		}
	}
}

void ACSPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		FVector2D MovementVector = InValue.Get<FVector2D>();

		const FRotator ControlRotation = GetController()->GetControlRotation();
		const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

		const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardVector, MovementVector.X);
		AddMovementInput(RightVector, MovementVector.Y);
	}
}

void ACSPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		FVector2D LookVector = InValue.Get<FVector2D>();

		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);	
	}
}

void ACSPlayerCharacter::StartSprint(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldSprint = InValue.Get<bool>();

		if (bShouldSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
	}
}

void ACSPlayerCharacter::StopSprint(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldSprint = InValue.Get<bool>();

		if (!bShouldSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
		}
	}
}

void ACSPlayerCharacter::StartCrouch(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldCrouch = InValue.Get<bool>();

		if (bShouldCrouch)
		{
			Crouch();
			bIsCrouching = true;
		}
	}
}

void ACSPlayerCharacter::StopCrouch(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldCrouch = InValue.Get<bool>();

		if (!bShouldCrouch)
		{
			UnCrouch();
			bIsCrouching = false;
		}
	}
}

void ACSPlayerCharacter::StartIronSight(const FInputActionValue& InValue)
{
	bIsAttackKeyPressed = true;
	TargetFOV = 45.f;
}

void ACSPlayerCharacter::StopIronSight(const FInputActionValue& InValue)
{
	bIsAttackKeyPressed = false;
	TargetFOV = 70.f;
}

void ACSPlayerCharacter::Reload()
{
	// 변경: !bIsNowAttacking && !bIsDead 대신, 현재 상태가 '평시'인지 확인합니다.
	if (Bullet < MaxBullet && GetCurrentState() == ECharacterState::Idle)
	{
		UCSPlayerAnimInstance* AnimInstance =
			Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

		if (IsValid(AnimInstance) && IsValid(ReloadMontage))
		{
			// 변경: 재장전 중에는 다른 행동을 막기 위해 상태를 'Attacking'으로 설정합니다.
			// (더 명확하게 하려면 ECharacterState에 Reloading 상태를 추가할 수도 있습니다.)
			SetCurrentState(ECharacterState::Attacking);
			AnimInstance->Montage_Play(ReloadMontage);

			if (OnReloadMontageEndedDelegate.IsBound() == false)
			{
				OnReloadMontageEndedDelegate.BindLambda([this](UAnimMontage* InMontage, bool bInterrupted)
				{
					if (!bInterrupted)
					{
						Bullet = MaxBullet;
						OnBulletChanged.Broadcast(Bullet);
						UE_LOG(LogTemp, Warning, TEXT("Reloaded, Ammo: %d"), Bullet);
					}
					// 변경: 재장전이 끝나면 다시 '평시' 상태로 돌아옵니다.
					SetCurrentState(ECharacterState::Idle);
					OnReloadMontageEndedDelegate.Unbind();
				});
				
				AnimInstance->Montage_SetEndDelegate(OnReloadMontageEndedDelegate, ReloadMontage);
			}
		}
		
	}
}

float ACSPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (GetIsDead())
	{
		IsDying();
	}
	else
	{
		// 부분 레그돌 판정 경우
		// FName PivotBoneName = FName(TEXT("pelvis"));
		// GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
		// TargetRagdollBlendWeight = 1.f;
		//
		// HittedRagdollRestoreTimerDelegate.BindUObject(
		// 	this, &ThisClass::OnHittedRagdollRestoreTimerElapsed
		// );
		//
		// GetWorldTimerManager().SetTimer(
		// 	HittedRagdollRestoreTimer,
		// 	HittedRagdollRestoreTimerDelegate,
		// 	1.f,
		// 	false
		// );

		// 피격 애니메이션 재생 경우
		UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
		
		if (IsValid(AnimInstance) && IsValid(OnHitMontage)
			&& AnimInstance->Montage_IsPlaying(OnHitMontage) == false)
		{
			AnimInstance->Montage_Play(OnHitMontage);
		}
	}

	return FinalDamageAmount;
}

void ACSPlayerCharacter::InputShoot(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldShoot = InValue.Get<bool>();

		if (bShouldShoot)
		{
			bIsAttackKeyPressed = true;
			// 변경: !bIsNowAttacking && !bIsDead 대신, 현재 상태가 '평시'인지 확인합니다.
			if (GetCurrentState() == ECharacterState::Idle)
			{
				if (ConsumeBullet())
				{
					BeginAttack();	// 애니메이션 재생
					
				}
			}
		}
	}
}

void ACSPlayerCharacter::InputReload(const FInputActionValue& InValue)
{
	if (IsValid(GetController()))
	{
		bool bShouldReload = InValue.Get<bool>();

		if (bShouldReload)
		{
			Reload();
		}
	}
}

bool ACSPlayerCharacter::ConsumeBullet()
{
	if (Bullet > 0)
	{
		Bullet--;
		OnBulletChanged.Broadcast(Bullet);
		
		UE_LOG(LogTemp, Error, TEXT("Shoot! Ammo: %d"), Bullet);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("No Ammo"));
	return false;
}

void ACSPlayerCharacter::TryFire()
{
	// 궁극기 캐스팅 중이면 사격 불가
	// if (UltCastState == EUltCastState::Casting)
	// {
	// 	return;
	// }
	
    if (!IsValid(GetController()))
    {
        return;
    }

    // 1. 카메라와 총구 위치 계산
    FVector CameraLocation, WeaponLocation;
    FRotator CameraRotation;
    GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
    WeaponLocation = GetMesh()->GetSocketLocation(TEXT("FX_Gun_Barrel"));

    // 2. 카메라 조준선 계산
    FVector CameraForward = CameraRotation.Vector();
    FVector TraceEnd = CameraLocation + (CameraForward * MaxShootRange);

    // 3. 조준점 찾기
    FHitResult AimHitResult;
    FCollisionQueryParams AimTraceParams(NAME_None, true, this);
    AimTraceParams.AddIgnoredActor(this);

    bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
        AimHitResult,
        CameraLocation,
        TraceEnd,
        ECC_Visibility,
        AimTraceParams
    );

    // 4. 최종 타겟 위치 결정
    FVector TargetLocation = bHitSomething ? AimHitResult.ImpactPoint : TraceEnd;

    // 5. 총구 오프셋 보정 계산
    FVector WeaponToTarget = TargetLocation - WeaponLocation;
    float DistanceToTarget = WeaponToTarget.Size();
    float WeaponOffsetFromCamera = FVector::Dist2D(WeaponLocation, CameraLocation);
    
    // 6. 발사 방향 계산
    float HorizontalCompensationAngle = FMath::Atan2(WeaponOffsetFromCamera, DistanceToTarget);
    FVector DirectionToTarget = WeaponToTarget.GetSafeNormal();
    FRotator TargetRotation = DirectionToTarget.Rotation();
    TargetRotation.Yaw += FMath::RadiansToDegrees(HorizontalCompensationAngle * 0.02f);
    FVector BulletDirection = TargetRotation.Vector();

    // 7. 실제 발사 라인트레이스
    FVector EndLocation = WeaponLocation + BulletDirection * MaxShootRange;
    FHitResult FireHitResult;
    FCollisionQueryParams FireTraceParams(NAME_None, true, this);
    FireTraceParams.AddIgnoredActor(this);

    bool bFireHit = GetWorld()->LineTraceSingleByChannel(
        FireHitResult,
        WeaponLocation,
        EndLocation,
        ECC_SHOOT,
        FireTraceParams
    );

    // 8. 디버그 시각화
    if (ShowAttackRangedDebug > 0)
    {
        DrawDebugVisualization(WeaponLocation, CameraLocation, TargetLocation, 
            FireHitResult, bFireHit, EndLocation, ShowAttackRangedDebug);
    }

    // 9. 데미지 처리
    if (bFireHit)
    {
        ProcessHit(FireHitResult);
    }
}

// 디버그 시각화를 위한 헬퍼 함수
void ACSPlayerCharacter::DrawDebugVisualization(
    const FVector& WeaponLocation,
    const FVector& CameraLocation,
    const FVector& TargetLocation,
    const FHitResult& HitResult,
    bool bHit,
    const FVector& EndLocation,
    int32 DebugType)
{
    if (1 == DebugType)
    {
        // 기본 디버그 정보
        DrawDebugSphere(GetWorld(), WeaponLocation, 5.f, 16, FColor::Red, false, 10.f);
        DrawDebugSphere(GetWorld(), CameraLocation, 5.f, 16, FColor::Yellow, false, 10.f);
        DrawDebugSphere(GetWorld(), TargetLocation, 5.f, 16, FColor::Magenta, false, 10.f);
        DrawDebugLine(GetWorld(), WeaponLocation, CameraLocation, FColor::Yellow, false, 10.f, 0, 5.f);
        DrawDebugLine(GetWorld(), CameraLocation, TargetLocation, FColor::Blue, false, 10.f, 0, 5.f);
        DrawDebugLine(GetWorld(), WeaponLocation, TargetLocation, FColor::Red, false, 10.f, 0, 5.f);
    }
    else if (2 == DebugType)
    {
        // 발사 궤적 디버그
        if (bHit)
        {
            DrawDebugSphere(GetWorld(), WeaponLocation, 5.f, 16, FColor::Red, false, 10.f);
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 16, FColor::Green, false, 10.f);
            DrawDebugLine(GetWorld(), WeaponLocation, HitResult.ImpactPoint, FColor::Blue, false, 10.f, 0, 5.f);
        }
        else
        {
            DrawDebugSphere(GetWorld(), WeaponLocation, 5.f, 16, FColor::Red, false, 10.f);
            DrawDebugSphere(GetWorld(), EndLocation, 5.f, 16, FColor::Green, false, 10.f);
            DrawDebugLine(GetWorld(), WeaponLocation, EndLocation, FColor::Blue, false, 10.f, 0, 5.f);
        }
    }
}

// 히트 처리를 위한 헬퍼 함수
void ACSPlayerCharacter::ProcessHit(const FHitResult& HitResult)
{
    ACSCharacterBase* HittedCharacter = Cast<ACSCharacterBase>(HitResult.GetActor());
    if (IsValid(HittedCharacter))
    {
    	UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticleSystem,
			HitResult.Location,
			HitResult.ImpactNormal.Rotation()
    	);
    	
        FDamageEvent DamageEvent;
        FString BoneNameString = HitResult.BoneName.ToString();
        
        DrawDebugSphere(GetWorld(), HitResult.Location, 3.f, 16, FColor(255,0,0,255), true, 20.f, 0U, 5.f);
        
        if (BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))
        {
            HittedCharacter->TakeDamage(AttackDamage * 3.f, DamageEvent, GetController(), this);
        }
        else
        {
            HittedCharacter->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
        }
    }

	else
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) && HitActor->CanBeDamaged())
		{
			FDamageEvent DamageEvent;
			HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}
}

void ACSPlayerCharacter::BeginAttack()
{
	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	// 변경: 상태를 'Attacking'으로 설정합니다.
	SetCurrentState(ECharacterState::Attacking);
	if (IsValid(AnimInstance) && IsValid(ShootMontage)
		&& AnimInstance->Montage_IsPlaying(ShootMontage) == false)
	{
		AnimInstance->Montage_Play(ShootMontage);
	}

	if (OnShootMontageEndedDelegate.IsBound() == false)
	{
		OnShootMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnShootMontageEndedDelegate, ShootMontage);
	}
}

void ACSPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterrupted)
{
	bIsAttackKeyPressed = false;
	// 변경: 상태를 다시 '평시'로 설정합니다.
	SetCurrentState(ECharacterState::Idle);

	if (OnShootMontageEndedDelegate.IsBound() == true)
	{
		OnShootMontageEndedDelegate.Unbind();
	}
}

void ACSPlayerCharacter::OnHittedRagdollRestoreTimerElapsed()
{
	FName PivotBoneName = FName(TEXT("pelvis"));

	TargetRagdollBlendWeight = 0.f;
	CurrentRagdollBlendWeight = 1.f;
	bIsNowRagdollBlending = true;
}

void ACSPlayerCharacter::IsDying()
{
	if (!IsValid(GetMesh()))
	{
		UE_LOG(LogTemp, Error, TEXT("[IsDying] SkeletalMeshComponent가 유효하지 않습니다"));
		return;
	}

	UAnimInstance* RawInstance = GetMesh()->GetAnimInstance();
	UCSPlayerAnimInstance* AnimInstance = Cast<UCSPlayerAnimInstance>(RawInstance);

	if (!IsValid(AnimInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("[IsDying] AnimInstance가 NULL입니다: %s"), *GetName());
		return;
	}

	if (IsValid(DeathMontage)
		&& !AnimInstance->Montage_IsPlaying(DeathMontage)
		&& CurrentState == ECharacterState::Dead)
	{
		AnimInstance->OnPostDead.RemoveAll(this);
		AnimInstance->OnPostDead.AddDynamic(this, &ACSCharacterBase::HandleOnPostDead);	
		
		AnimInstance->Montage_Play(DeathMontage);
	}
}

FTransform ACSPlayerCharacter::CalculateLeftHandIKGoalTransform()
{
	FTransform HandLWorldTransform = GetMesh()->GetSocketTransform(TEXT("hand_l"));

	FTransform WeaponLRelativeToHandTransform =
		FTransform(FRotator(18.1, 80.3, 164.7), FVector(10.4, -1.7, 0.3)); // 손에 총이 잡히는 상대 위치/회전

	return WeaponLRelativeToHandTransform * HandLWorldTransform; // hand_l 기준으로 weapon_l의 목표 위치 계산
}

void ACSPlayerCharacter::OnUltCastPressed(const FInputActionValue& InValue)
{
	if (UltCastState == EUltCastState::Casting || UltCastState == EUltCastState::CoolDown)
	{
		return;
	}

	// 캐스팅 시작
	UltCastState = EUltCastState::Casting;
	CurrentCastTime = 0.f;

	// 파티클 재생
	StartCastingEffect();

	GetCharacterMovement()->MaxWalkSpeed = 150.f;

	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	if (IsValid(AnimInstance) && IsValid(UltCastingMontage))
	{
		if (!AnimInstance->Montage_IsPlaying(UltCastingMontage))
		{
			AnimInstance->Montage_Play(UltCastingMontage);
			AnimInstance->Montage_JumpToSection(UltCastStartSectionName, UltCastingMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(UltCastLoopSectionName, UltCastingMontage);
		}
	}

	if (IsValid(UltCastSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			UltCastSound,
			GetActorLocation()
		);
	}

	// 파티클 이펙트 스폰
	// if (IsValid(UltCastParticleEffect))
	// {
	// 	UGameplayStatics::SpawnEmitterAtLocation(
	// 		GetWorld(),
	// 		UltCastParticleEffect,
	// 		GetMesh()->GetSocketLocation(TEXT("hand_r_ability_socket")),
	// 		GetActorRotation()
	// 	);
	// }

	// 캐스팅 타이머 시작
	TimerDelegateForMaxCast.BindLambda([this]()
	{
		UltCastState = EUltCastState::ReadyToFire;
	});
	GetWorldTimerManager().SetTimer(
		CastTimerHandle,
		TimerDelegateForMaxCast,
		MaxCastTime,
		false
	);

	TimerDelegateForCastUpdate.BindLambda([this]()
	{
		if (UltCastState == EUltCastState::Casting)
		{
			CurrentCastTime += GetWorldTimerManager().GetTimerRate(CastUpdateTimerHandle);
		}
	});
	GetWorldTimerManager().SetTimer(
		CastUpdateTimerHandle,
		TimerDelegateForCastUpdate,
		0.05f,
		true
	);
	
}

void ACSPlayerCharacter::OnUltCastReleased(const FInputActionValue& InValue)
{
	if (UltCastState != EUltCastState::Casting
		&& UltCastState != EUltCastState::ReadyToFire)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(CastTimerHandle);
	GetWorldTimerManager().ClearTimer(CastUpdateTimerHandle);

	// 파티클 재생 종료
	StopCastingEffect();

	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	if (CurrentCastTime >= MinCastTime)
	{
		// 캐스팅 애니메이션 몽타주 종료
		if (IsValid(AnimInstance) && AnimInstance->Montage_IsPlaying(UltCastingMontage))
		{
			AnimInstance->Montage_JumpToSection(UltCastEndSectionName, UltCastingMontage);
		}
		// 사출
		FireUltEffect();
		
		UltCastState = EUltCastState::CoolDown;
		TimerDelegateForCoolDown.BindLambda([this]()
		{
			UltCastState = EUltCastState::None;
		});
		GetWorldTimerManager().SetTimer(
			CoolDownTimerHandle,
			TimerDelegateForCoolDown,
			CoolDownTime,
			false
		);
	}
	else
	{
		UltCastState = EUltCastState::None;

		if (IsValid(AnimInstance) && AnimInstance->Montage_IsPlaying(UltCastingMontage))
		{
			AnimInstance->Montage_Stop(0.25f, UltCastingMontage);
		}
	}
}

void ACSPlayerCharacter::FireUltEffect()
{
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 280.f + GetActorRightVector() * 70.f;
	FRotator SpawnRotation = GetActorRotation();

	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));
	
	if (IsValid(AnimInstance) && IsValid(UltFireMontage)
		&& AnimInstance->Montage_IsPlaying(UltFireMontage) == false)
	{
		AnimInstance->Montage_Play(UltFireMontage);
	}
	
	// 파티클 이펙트 스폰
	if (IsValid(UltFireParticleEffect))
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			UltFireParticleEffect,
			SpawnLocation,
			FRotator(SpawnRotation.Pitch, SpawnRotation.Yaw - 90.f, SpawnRotation.Roll)
		);
	}

	// 사운드 재생
	if (IsValid(UltFireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			UltFireSound,
			SpawnLocation
		);
	}

	// 투사체 발사
	if (IsValid(UltProjectileClass))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 투사체 스폰
		ACSUltProjectile* Projectile =
			GetWorld()->SpawnActor<ACSUltProjectile>(UltProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (IsValid(Projectile) && Projectile->ProjectileMovement)
		{
			FVector LaunchDirection = SpawnRotation.Vector();
			Projectile->ProjectileMovement->Velocity = LaunchDirection * Projectile->ProjectileMovement->InitialSpeed;
		}
	}
	StopCastingEffect();
}

void ACSPlayerCharacter::StartCastingEffect()
{
	StopCastingEffect();

	if (UltCastParticleEffect)
	{
		UltCastParticleEffectComponent = UGameplayStatics::SpawnEmitterAttached(
			UltCastParticleEffect,
			GetMesh(),
			TEXT("hand_r_ability_socket"),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
}

void ACSPlayerCharacter::StopCastingEffect()
{
	if (IsValid(UltCastParticleEffectComponent))
	{
		UltCastParticleEffectComponent->DeactivateSystem();
		UltCastParticleEffectComponent = nullptr;
	}
}

void ACSPlayerCharacter::TryActivateNearbyItem()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->Implements<UItemInterface>())
		{
			IItemInterface::Execute_ActivateItem(Actor, this);
			UE_LOG(LogTemp, Warning, TEXT("Press F: %s"), *Actor->GetName());
			break;
		}
	}
}




