#include "CSPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/CSInputConfig.h"
#include "Animation/CSPlayerAnimInstance.h"

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
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 25.f));

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
	if (Bullet < MaxBullet)
	{
		Bullet = MaxBullet;
		OnBulletChanged.Broadcast(Bullet);
		UE_LOG(LogTemp, Warning, TEXT("Reloaded, Ammo: %d"), Bullet);
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
			if (!bIsNowAttacking && !bIsDead)
			{
				if (ConsumeBullet())
				{
					BeginAttack();
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

void ACSPlayerCharacter::BeginAttack()
{
	UCSPlayerAnimInstance* AnimInstance =
		Cast<UCSPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance), TEXT("Invalid AnimInstnace"));
	
	bIsNowAttacking = true;
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

void ACSPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnShootMontageEndedDelegate.IsBound() == true)
	{
		OnShootMontageEndedDelegate.Unbind();
	}
}





