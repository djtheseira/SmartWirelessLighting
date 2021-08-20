// ILikeBanas

#pragma once

#include "CoreMinimal.h"
#include "FGCharacterPlayer.h"
#include "Buildables/FGBuildableWire.h"
#include "Build_SmartWirelessWireBase.generated.h"

/**
 * 
 */
UCLASS()
class SMARTWIRELESSLIGHTING_API ABuild_SmartWirelessWireBase : public AFGBuildableWire
{
	GENERATED_BODY()
public:
	ABuild_SmartWirelessWireBase();

	// Begin AActor interface
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void Serialize(FArchive& ar) override;
	virtual void BeginPlay() override;
	//virtual void Destroyed() override;
	
	// Begin IFGDismantleInterface
	//virtual void Dismantle_Implementation() override;
	virtual void GetDismantleRefund_Implementation(TArray< FInventoryStack >& out_refund) const override;
	// End IFGDismantleInterface
};
