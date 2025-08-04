#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSFireBomb.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class TEAM01_API ACSFireBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSFireBomb();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *StaticMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* ProjectileComp;

	void LaunchProjectile(const FVector& Direction, float Speed);

	UPROPERTY(EditAnywhere, Category = "Effect")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage;

	UPROPERTY()
	AController* InstigatorController;

	UPROPERTY()
	APawn* PlayerPawn;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
