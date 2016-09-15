// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"

#include "CPP_TutorialCharacter.generated.h"

UCLASS(config=Game)
class ACPP_TutorialCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACPP_TutorialCharacter();

	/* Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/* Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/*
	*	Function to update the character's power.
	*	@param powerChange - This is the amount to change the power by, and it can be positive or negative.
	*/
	UFUNCTION(BlueprintCallable, Category = "Power")
	void UpdatePower(float powerChange);

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetInitialPower() { return InitialPower; }

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetCurrentPower() { return _currentPowerLevel; }

	/* Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return _cameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return _followCamera; }
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return _collectionSphere; }

protected:
	/* The starting power level of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float InitialPower;

	/* Multiplier for character speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float SpeedFactor;

	/* Speed when power level = 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float BaseSpeed;

	/* Resets HMD orientation in VR. */
	void OnResetVR();

	/* Called for forwards/backward input */
	void MoveForward(float Value);

	/* Called for side to side input */
	void MoveRight(float Value);

	/* 
	 *	Called via input to turn at a given rate. 
	 *	@param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/*
	 *	Called via input to turn look up/down at a given rate. 
	 *	@param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/* Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/* Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/* Called when we press a key. */
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void CollectPickups();

	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
	void PowerChangeEffect();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:
	/* Current power level of the character. */
	UPROPERTY(VisibleAnywhere, Category = "Power")
	float _currentPowerLevel;

	/* Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* _cameraBoom;

	/* Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* _followCamera;

	/* Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* _collectionSphere;
};
