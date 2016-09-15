// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"

#include "CPP_TutorialGameMode.generated.h"

// enum to store the current state of gameplay
UENUM(BlueprintType)
enum class BatteryPlayState { Playing, GameOver, Won, Unknown };

UCLASS(minimalapi)
class ACPP_TutorialGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACPP_TutorialGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/* Sets a new playing state. */
	void SetCurrentState(BatteryPlayState newState);

	/* Returns the power needed to win. */
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const { return PowerToWin; }
	
	/* Returns the current playing state. */
	UFUNCTION(BlueprintPure, Category = "Power")
	BatteryPlayState GetCurrentState() const { return _currentState; }

protected:
	/* The rate at which the character loses power. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float DecayRate;

	/* The power level needed to trigger the game won state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float PowerToWin;

	/* The instance of the HUD widget. */
	UPROPERTY()
	class UUserWidget* CurrentWidget;

	/* The Widget class to use for our HUD screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

private:
	/* Keeps track of the current playing state. */
	BatteryPlayState _currentState;

	/* Container of all Spawn Volumes in the current level. */
	TArray<class ASpawnVolume*> _spawnVolumeActors;

	/* Handle any function calls that rely upon changes to the playing state. */
	void _handleNewState(BatteryPlayState newState);
};
