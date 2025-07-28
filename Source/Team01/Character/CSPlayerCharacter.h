#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CSPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCSInputConfig;
class UInputMappingContext;

UCLASS()
class TEAM01_API ACSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter
	
public:
	ACSPlayerCharacter();

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputJump(const FInputActionValue& InValue);
	void StartSprint(const FInputActionValue& InValue);
	void StopSprint(const FInputActionValue& InValue);
	void StartCrouch(const FInputActionValue& InValue);
	void StopCrouch(const FInputActionValue& InValue);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCSInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

#pragma endregion

};
