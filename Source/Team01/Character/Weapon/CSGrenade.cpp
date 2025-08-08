// Fill out your copyright notice in the Description page of Project Settings.


#include "../Weapon/CSGrenade.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


// Sets default values
ACSGrenade::ACSGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	Collision->InitSphereRadius(40.f);
	Collision->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = Collision;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(Collision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovement->UpdatedComponent = Collision;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.5f;
	ProjectileMovement->ProjectileGravityScale = 1.f;

	ExplosionRadius = 500.f;
	Damage = 30.f;
	ExplosionDelay = 3.f;

}

// Called when the game starts or when spawned
void ACSGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// void ACSGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
// 	FVector NormalImpulse, const FHitResult& Hit)
// {
// 	
// }

void ACSGrenade::Throw()
{
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ThisClass::Explode,
		ExplosionDelay,
		false
	);
}

void ACSGrenade::Explode()
{
	if (IsValid(ExplosionEffect))
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation()
		);
	}
	if (IsValid(ExplosionSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	UGameplayStatics::ApplyRadialDamage(
		this,								// DamageCauser
		Damage,								// 폭발 중심 데미지
		GetActorLocation(),					// 폭발 원점
		ExplosionRadius,					// 폭발 반경
		UDamageType::StaticClass(),			// 데미지 타입: 기본
		TArray<AActor*>(),					// 무시할 액터 배열
		this,								// 데미지 발생자
		GetInstigatorController(),			// 데미지 유발 컨트롤러
		false,					
		ECollisionChannel::ECC_Visibility	// 라인 트레이스용 적절한 콜리전 채널
	);

	Destroy();
}


