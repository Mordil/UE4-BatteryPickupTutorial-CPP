#include "CPP_Tutorial.h"

#include "Pickup.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	IsActive = true;

	// Create the static mesh component
	_pickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = _pickupMesh;
}

void APickup::WasCollected_Implementation()
{
	FString pickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You have collected &s"), *pickupDebugString)
}

void APickup::SetActive(bool isActive)
{
	IsActive = isActive;
}
