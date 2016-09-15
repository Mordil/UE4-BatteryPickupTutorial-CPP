#pragma once

#include "GameFramework/Actor.h"

#include "SpawnVolume.generated.h"

UCLASS()
class CPP_TUTORIAL_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

	/* This toggles whether or not the spawn volume spawns pickups. */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool shouldSpawn);

	/* Find a random point within the Spawning Volume. */
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointsInVolume();

	/* Returns the Spawning Volume subobject. */
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return _spawningVolume; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayMax;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> PickupToSpawn;

	FTimerHandle SpawnTimer;

private:
	/* The current spawn delay. */
	float _spawnDelay;

	/* Box component to represent the area in space pickups can spawn from. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _spawningVolume;
	
	/* Handle spawning a new pickup. */
	void _spawnPickup();

	/* Binds SpawnPickup() to a timer to trigger between the Spawn Delay min & max. */
	void _setSpawnTimer();
};
