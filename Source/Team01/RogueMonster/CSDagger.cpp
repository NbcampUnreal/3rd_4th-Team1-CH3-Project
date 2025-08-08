#include "CSDagger.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ACSDagger::ACSDagger()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 2500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 1.0f;

	Damage = 20.0f;
}

void ACSDagger::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
	{
		Mesh->OnComponentHit.AddDynamic(this, &ACSDagger::OnHit);
	}
}

void ACSDagger::SetDirection(const FVector& Direction)
{
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
	SetActorRotation(Direction.Rotation());
}

void ACSDagger::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) return;

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	Destroy();
}