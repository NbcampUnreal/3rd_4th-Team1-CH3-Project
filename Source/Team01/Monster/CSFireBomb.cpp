#include "CSFireBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"


ACSFireBomb::ACSFireBomb()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	StaticMesh->SetNotifyRigidBodyCollision(true);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->InitialSpeed = 0.f;
	ProjectileComp->MaxSpeed = 2000.f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->ProjectileGravityScale = 1.f;

	AttackDamage = 10.0f;
}

void ACSFireBomb::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->OnComponentHit.AddDynamic(this, &ACSFireBomb::OnHit);

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);	
}

void ACSFireBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSFireBomb::LaunchProjectile(const FVector& Direction, float Speed)
{
	if (StaticMesh)
	{
		ProjectileComp->Velocity = Direction * Speed;
	}

}

void ACSFireBomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor == PlayerPawn)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			AttackDamage,
			InstigatorController,
			this,
			UDamageType::StaticClass()
		);
	}

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	Destroy();
}