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
#include "Materials/MaterialExpressionBlendMaterialAttributes.h"


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
	TargetFOV = 45.f;
}

void ACSPlayerCharacter::StopIronSight(const FInputActionValue& InValue)
{
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
	if (IsValid(GetController()))
	{
#pragma region CalculateTargetTransform

		float FocalDistance = 400.f;	// 조준 거리
		FVector FocalLocation, CameraLocation;	// 초점 위치, 카메라 위치
		FRotator CameraRotation;	// 카메라 회전값

		// '플레이어의 시점' 을 통해 카메라 위치와 회전 Get
		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// 카메라가 바라보는 방향
		FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
		// 조준점 위치 계산
		FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

		// 무기 위치를 확인 => 발사 위치 지정
		FName WeaponSocketName = TEXT("FX_Gun_Barrel");
		FVector WeaponLocation = GetMesh()->GetSocketLocation(WeaponSocketName);

		// 최종 조준점 위치
		FVector FinalFocalLocation =
			FocalLocation + (((WeaponLocation - FocalLocation) | AimDirectionFromCamera) * AimDirectionFromCamera);

		FTransform TargetTransform(CameraRotation, FinalFocalLocation);

		if (1 == ShowAttackRangedDebug)
		{
			DrawDebugSphere(GetWorld(), WeaponLocation, 5.f, 16, FColor::Red, false, 10.f);
			DrawDebugSphere(GetWorld(), CameraLocation, 5.f, 16, FColor::Yellow, false, 10.f);
			DrawDebugSphere(GetWorld(), FinalFocalLocation, 5.f, 16, FColor::Magenta, false, 10.f);

			DrawDebugLine(GetWorld(), FocalLocation, WeaponLocation, FColor::Yellow, false, 10.f, 0, 5.f);
			DrawDebugLine(GetWorld(), CameraLocation, FinalFocalLocation, FColor::Blue, false, 10.f, 0, 5.f);
			DrawDebugLine(GetWorld(), WeaponLocation, FinalFocalLocation, FColor::Red, false, 10.f, 0, 5.f);
		}

#pragma endregion

#pragma region PerformLineTracing

		FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);

		// 우측 탄도학 적용
		FVector RightVector = TargetTransform.GetUnitAxis(EAxis::Y);
		BulletDirection += RightVector * 0.05f;
		BulletDirection = BulletDirection.GetSafeNormal();
		
		FVector StartLocation = WeaponLocation;
		FVector EndLocation = StartLocation + BulletDirection * GetMaxShootRange();

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, true, this);	// 의도적으로 시작부터 겹쳐도 사격가능하게 설정
		TraceParams.AddIgnoredActor(this);	// 자기 자신 무시

		bool IsCollided = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_SHOOT,
			TraceParams
		);

		// 충돌이 없을 경우
		if (IsCollided == false)
		{
			HitResult.TraceStart = StartLocation;
			HitResult.TraceEnd = EndLocation;
		}

		// 실제 발사체 충돌 검사
		if (2 == ShowAttackRangedDebug)
		{
			if (IsCollided)
			{
				DrawDebugSphere(GetWorld(), StartLocation, 5.f, 16, FColor::Red, false, 10.f);
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 16, FColor::Green, false, 10.f);

				DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 10.f, 0, 5.f);
			}
			else
			{
				DrawDebugSphere(GetWorld(), StartLocation, 5.f, 16, FColor::Red, false, 10.f);
				DrawDebugSphere(GetWorld(), EndLocation, 5.f, 16, FColor::Green, false, 10.f);

				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 10.f, 0, 5.f);
			}
		}

#pragma endregion

		// 착탄 성공 시 TakeDamage
		if (IsCollided)
		{
			ACSCharacterBase* HittedCharacter =
				Cast<ACSCharacterBase>(HitResult.GetActor());
			if (IsValid(HittedCharacter))
			{
				FDamageEvent DamageEvent;

				// 피격 부위 확인
				FString BoneNameString = HitResult.BoneName.ToString();
				UKismetSystemLibrary::PrintString(this, BoneNameString);
				DrawDebugSphere(GetWorld(), HitResult.Location,
					3.f, 16, FColor(255,0,0,255), true, 20.f, 0U, 5.f);

				if (BoneNameString.Equals(FString(TEXT("HEAD")), ESearchCase::IgnoreCase))
				{
					HittedCharacter->TakeDamage(AttackDamage * 3.f, DamageEvent, GetController(), this);
				}
				else
				{
					HittedCharacter->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
				}
			}
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
	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstance"));

	if (IsValid(AnimInstance) && IsValid(DeathMontage)
		&& AnimInstance->Montage_IsPlaying(DeathMontage) == false
		&& CurrentState == ECharacterState::Dead)
	{
		AnimInstance->OnPostDead.RemoveAll(this);
		AnimInstance->OnPostDead.AddDynamic(this, &ACSCharacterBase::HandleOnPostDead);	
		
		AnimInstance->Montage_Play(DeathMontage);
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




