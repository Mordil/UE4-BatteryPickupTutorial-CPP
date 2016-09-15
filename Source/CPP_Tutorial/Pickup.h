#pragma once

#include "GameFramework/Actor.h"

#include "Pickup.generated.h"

UCLASS()
class CPP_TUTORIAL_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	
	/* Function to call when the pickup is collected. */
	UFUNCTION(BlueprintNativeEvent)
	void WasCollected();
	virtual void WasCollected_Implementation();

	/* Allows other classes to safely change whether or not the pickup is active. */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool isActive);

	/** Return wether or not the pickup is active. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool GetIsActive() { return IsActive; }

	/** Return the mesh for the pickup. */
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return _pickupMesh; }

protected:
	/** True when the pickup can be used, and false when pickup is deactivated. */
	bool IsActive;

private:
	/* Static mesh to represent the pickup in the level. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _pickupMesh;
};
