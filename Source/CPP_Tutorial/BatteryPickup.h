#pragma once

#include "Pickup.h"

#include "BatteryPickup.generated.h"

UCLASS()
class CPP_TUTORIAL_API ABatteryPickup : public APickup
{
	GENERATED_BODY()

public:
	ABatteryPickup();
	~ABatteryPickup();

	/* Override the WasCollectedImplementation function because it's a Blueprint Native Event. */
	void WasCollected_Implementation() override;

	/* Public way to access the battery's power level*/
	float GetPower() { return BatteryPower; }

protected:
	/* The amount of power the battery gives to the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", meta = (BlueprintProtected = "true"))
	float BatteryPower;
};
