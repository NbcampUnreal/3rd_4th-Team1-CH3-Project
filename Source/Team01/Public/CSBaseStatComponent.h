#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSBaseStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM01_API UCSBaseStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSBaseStatComponent();
	virtual void TakeDamage(float Damage);

protected: 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHp; // 최대 체력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float CurrentHp; //현재 체력

protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
};
