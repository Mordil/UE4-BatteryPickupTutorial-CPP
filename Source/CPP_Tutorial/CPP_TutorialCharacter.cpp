// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CPP_Tutorial.h"

#include "Kismet/HeadMountedDisplayFunctionLibrary.h"

#include "CPP_TutorialCharacter.h"
#include "BatteryPickup.h"

ACPP_TutorialCharacter::ACPP_TutorialCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	_cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_cameraBoom->SetupAttachment(RootComponent);
	_cameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	_cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	_followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_followCamera->SetupAttachment(_cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	_followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create the collection sphere
	_collectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	_collectionSphere->SetupAttachment(RootComponent);
	_collectionSphere->SetSphereRadius(200.0f);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// set the base power level
	InitialPower = 2000.0f;
	_currentPowerLevel = InitialPower;

	SpeedFactor = 0.75f;
	BaseSpeed = 10.0f;
}

void ACPP_TutorialCharacter::UpdatePower(float PowerChange)
{
	_currentPowerLevel += PowerChange;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * _currentPowerLevel;
	PowerChangeEffect();
}

void ACPP_TutorialCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACPP_TutorialCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACPP_TutorialCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACPP_TutorialCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ACPP_TutorialCharacter::CollectPickups);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPP_TutorialCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPP_TutorialCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACPP_TutorialCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACPP_TutorialCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACPP_TutorialCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACPP_TutorialCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACPP_TutorialCharacter::OnResetVR);
}

void ACPP_TutorialCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACPP_TutorialCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACPP_TutorialCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ACPP_TutorialCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ACPP_TutorialCharacter::CollectPickups()
{
	// Get pointers to all overlapping Actors and store them in an array
	TArray<AActor*> collectedActors;
	_collectionSphere->GetOverlappingActors(collectedActors);

	// keep track of the collected battery power
	float collectedPower = 0;

	for (int32 i = 0; i < collectedActors.Num(); ++i)
	{
		APickup* const TestPickup = Cast<APickup>(collectedActors[i]);

		// if the cast is successful and the pickup is valid + active
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->GetIsActive())
		{
			// Collect and deactivate the pickup
			TestPickup->WasCollected();

			// Test to see if the pickup is a battery
			ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(TestPickup);
			if (TestBattery)
			{
				// increase the collected power;
				collectedPower += TestBattery->GetPower();
			}

			TestPickup->SetActive(false);
		}
	}

	if (collectedPower > 0)
	{
		UpdatePower(collectedPower);
	}
}
