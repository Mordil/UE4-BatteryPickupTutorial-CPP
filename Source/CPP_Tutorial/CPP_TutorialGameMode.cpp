// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CPP_Tutorial.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "CPP_TutorialGameMode.h"
#include "CPP_TutorialCharacter.h"
#include "SpawnVolume.h"

ACPP_TutorialGameMode::ACPP_TutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	DecayRate = .01f;
}

void ACPP_TutorialGameMode::BeginPlay()
{
	Super::BeginPlay();

	// find all spawn volume Actors
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), foundActors);

	for (auto actor : foundActors)
	{
		ASpawnVolume* spawnVolumeActor = Cast<ASpawnVolume>(actor);
		if (spawnVolumeActor)
		{
			_spawnVolumeActors.AddUnique(spawnVolumeActor);
		}
	}

	SetCurrentState(BatteryPlayState::Playing);

	// set the score to beat
	ACPP_TutorialCharacter* MyCharacter = Cast<ACPP_TutorialCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		PowerToWin = (MyCharacter->GetInitialPower()) * 1.25f;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ACPP_TutorialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ACPP_TutorialCharacter* MyCharacter = Cast<ACPP_TutorialCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		if (MyCharacter->GetCurrentPower() > PowerToWin)
		{
			SetCurrentState(BatteryPlayState::Won);
		}
		else if (MyCharacter->GetCurrentPower() > 0)
		{
			MyCharacter->UpdatePower(-DeltaTime * DecayRate * (MyCharacter->GetInitialPower()));
		}
		else
		{
			SetCurrentState(BatteryPlayState::GameOver);
		}
	}
}

void ACPP_TutorialGameMode::SetCurrentState(BatteryPlayState newState)
{
	_currentState = newState;
	_handleNewState(_currentState);
}

void ACPP_TutorialGameMode::_handleNewState(BatteryPlayState newState)
{
	auto setVolumesActiveState = [&](bool valueToSet)
	{
		for (auto volume : _spawnVolumeActors)
		{
			volume->SetSpawningActive(valueToSet);
		}
	};

	switch (newState)
	{
	case BatteryPlayState::Playing:
	{
		setVolumesActiveState(true);
		break;
	}

	case BatteryPlayState::Won:
	{
		setVolumesActiveState(false);
		break;
	}

	case BatteryPlayState::GameOver:
	{
		setVolumesActiveState(false);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			PlayerController->SetCinematicMode(true, false, false, true, true);
		}

		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (MyCharacter)
		{
			MyCharacter->GetMesh()->SetSimulatePhysics(true);
			MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
		}
		break;
	}

	case BatteryPlayState::Unknown:
	default:
		break;
	}
}
