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
	
	GetCharacterMovement()->SetCrouchedHalfHeight(55.f);
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.f;

#pragma endregion

#pragma region Character Vision

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 450.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = true;
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
					BeginAttack();
					TryFire();
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

		float FocalDistance = 1000.f;
		FVector FocalLocation, CameraLocation;
		FRotator CameraRotation;

		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
		FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

		FTransform TargetTransform(CameraRotation, FocalLocation);

		if (1 == ShowAttackRangedDebug)
		{
			DrawDebugSphere(GetWorld(), FocalLocation, 100.f, 16, FColor::Red, false, 30.f);
			DrawDebugSphere(GetWorld(), CameraLocation, 100.f, 16, FColor::Yellow, false, 30.f);

			DrawDebugLine(GetWorld(), CameraLocation, FocalLocation, FColor::Blue, false, 30.f, 0, 5.f);
		}

#pragma endregion

#pragma region PerformLineTracing

		FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = TargetTransform.GetLocation() + BulletDirection * GetMaxShootRange();

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, false, this);
		TraceParams.AddIgnoredActor(this);

		bool IsCollided = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_SHOOT,
			TraceParams
		);
		if (!IsCollided)
		{
			HitResult.TraceStart = StartLocation;
			HitResult.TraceEnd = EndLocation;
		}

		if (2 == ShowAttackRangedDebug)
		{
			if (IsCollided)
			{
				DrawDebugSphere(GetWorld(), StartLocation, 100.f, 16, FColor::Red, false, 30.f);
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 100.f, 16, FColor::Green, false, 30.f);
				DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 30.f, 0, 5.f);
			}
			else
			{
				DrawDebugSphere(GetWorld(), StartLocation, 100.f, 16, FColor::Red, false, 30.f);
				DrawDebugSphere(GetWorld(), EndLocation, 100.f, 16, FColor::Green, false, 30.f);
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 30.f, 0, 5.f);
			}
		}

#pragma endregion

		if (IsCollided)
		{
			ACSCharacterBase* HittedCharacter =
				Cast<ACSCharacterBase>(HitResult.GetActor());
			if (IsValid(HittedCharacter))
			{
				FDamageEvent DamageEvent;
				HittedCharacter->TakeDamage(10.f, DamageEvent, GetController(), this);
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





