#include "CPP_Tutorial.h"

#include "BatteryPickup.h"

ABatteryPickup::ABatteryPickup()
{
	GetMesh()->SetSimulatePhysics(true);
	BatteryPower = 150.0f;
}

ABatteryPickup::~ABatteryPickup() { }

void ABatteryPickup::WasCollected_Implementation()
{
	// Use the base pickup behavior
	Super::WasCollected_Implementation();
	Destroy();
}
