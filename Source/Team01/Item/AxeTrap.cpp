#include "AxeTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AAxeTrap::AAxeTrap()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	AxeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AxeMesh"));
	AxeMesh->SetupAttachment(RootComponent);

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	DamageBox->SetupAttachment(AxeMesh);
	DamageBox->SetCollisionProfileName(TEXT("Trigger"));
	DamageBox->SetGenerateOverlapEvents(true);

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AAxeTrap::OnOverlapBegin);

	bCanDamage = true;
	RunningTime = 0.f;
}

void AAxeTrap::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = AxeMesh->GetRelativeRotation();
	
	TimeOffset = FMath::FRandRange(0.f, PI * 2.f);
	RotationSpeed = FMath::FRandRange(1.5f, 3.5f);
}

void AAxeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
	float SwingAngle = FMath::Sin(RunningTime * RotationSpeed + TimeOffset) * 60.f;
	FRotator NewRotation = InitialRotation + FRotator(SwingAngle, 0.f, 0.f); 
	AxeMesh->SetRelativeRotation(NewRotation);

}

void AAxeTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanDamage || !IsValid(OtherActor))
	{
		return;
	}
	
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		UGameplayStatics::ApplyDamage(Player, Damage, GetInstigatorController(), this, nullptr);
		
		FVector KnockbackDir = AxeMesh->GetRightVector();
		KnockbackDir.Z = 0.f;
		KnockbackDir.Normalize();
		FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
		if (FVector::DotProduct(KnockbackDir, ToPlayer) < 0)
		{
			KnockbackDir *= -1.f;
		}
		KnockbackDir += FVector(0, 0, 0.5f);
		KnockbackDir.Normalize();
		float KnockbackPower = 800.f;
		Player->LaunchCharacter(KnockbackDir * KnockbackPower, true, true);

		bCanDamage = false;
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this, &AAxeTrap::ResetDamage, InvincibilityDuration, false);
	}
}

void AAxeTrap::ResetDamage()
{
	bCanDamage = true;
}

