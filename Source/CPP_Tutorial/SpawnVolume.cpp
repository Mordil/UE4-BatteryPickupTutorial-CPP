/* This file needs to be the first include in all files. */
#include "CPP_Tutorial.h"

#include "Kismet/KismetMathLibrary.h"

#include "SpawnVolume.h"
#include "Pickup.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	_spawningVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = _spawningVolume;

	// Spawn delay defaults
	SpawnDelayMin = 1.0f;
	SpawnDelayMax = 4.5f;
}

void ASpawnVolume::SetSpawningActive(bool shouldSpawn)
{
	if (shouldSpawn)
	{
		_setSpawnTimer();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}

FVector ASpawnVolume::GetRandomPointsInVolume()
{
	FVector spawnOrigin = _spawningVolume->Bounds.Origin;
	FVector spawnExtent = _spawningVolume->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(spawnOrigin, spawnExtent);
}

void ASpawnVolume::_spawnPickup()
{
	if (PickupToSpawn != NULL)
	{
		UWorld* const world = GetWorld();

		if (world != NULL)
		{
			// Spawning boilerplate
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			spawnParams.Instigator = Instigator;

			auto getRandomRotationAxis = []() { return FMath::FRand() * 360.0f; };

			FVector spawnLocation = GetRandomPointsInVolume();
			FRotator spawnRotation = { getRandomRotationAxis(), getRandomRotationAxis(), getRandomRotationAxis() };

			APickup* const spawnedPickup = world->SpawnActor<APickup>(PickupToSpawn, spawnLocation, spawnRotation, spawnParams);

			_setSpawnTimer();
		}
	}
}

void ASpawnVolume::_setSpawnTimer()
{
	_spawnDelay = FMath::FRandRange(SpawnDelayMin, SpawnDelayMax);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::_spawnPickup, _spawnDelay, false);
}

