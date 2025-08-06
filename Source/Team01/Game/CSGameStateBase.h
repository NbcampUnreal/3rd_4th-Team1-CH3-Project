#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CSGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Prepare,
	KillEnemies,
	BossFight,
	MissionClear
};

UCLASS()
class TEAM01_API ACSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AddKill();

	UFUNCTION()
	void SetMissionState(EMissionState NewState);

	UFUNCTION(BlueprintCallable)
	int32 GetTotalKillCount() const { return TotalKillCount; }

	UFUNCTION(BlueprintCallable)
	EMissionState GetCurrentMissionState() const { return CurrentMissionState; }

	static FString GetMissionText(EMissionState State, int32 KillCount);
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	EMissionState CurrentMissionState = EMissionState::KillEnemies;
	
	UPROPERTY(VisibleAnywhere)
	int32 TotalKillCount = 0;

	
};
