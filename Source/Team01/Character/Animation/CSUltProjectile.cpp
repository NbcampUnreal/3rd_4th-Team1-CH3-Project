// Fill out your copyright notice in the Description page of Project Settings.


#include "../Animation/CSUltProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// Sets default values
ACSUltProjectile::ACSUltProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(40.f);
	RootComponent = CollisionComponent;

	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	Damage = 50.f;
	ProjectileLifeSpan = 5.f;
	InitialLifeSpan = ProjectileLifeSpan;
}

// Called when the game starts or when spawned
void ACSUltProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSUltProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSUltProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor) && OtherActor != this && IsValid(OtherComp))
	{
		// 데미지 적용
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);

		// 파티클, 사운드 등 이펙트
		if (IsValid(ProjectileHitEffect))
		{
			UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ProjectileHitEffect,
			GetActorLocation(),
			GetActorRotation()
			);
		}
		if (IsValid(ProjectileHitSound))
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				ProjectileHitSound,
				GetActorLocation()
			);
		}

		// 투사체 제거
		Destroy();
	}
}

