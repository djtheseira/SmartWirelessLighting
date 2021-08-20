#pragma once

#include "CoreMinimal.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGCircuitSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGInventoryComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FGUseableInterface.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "SmartLightsControlPanel.generated.h"

USTRUCT(Blueprintable)
struct SMARTWIRELESSLIGHTING_API FBuildableLightingConnection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class AFGBuildableLightSource* mBuildableLightSource = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	bool isConnected = false;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class AFGBuildableWire* mBuildableWire = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGCircuitConnectionComponent* mDownstreamConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGPowerConnectionComponent* mBuildablePowerConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	bool mShouldShow = false;

	FORCEINLINE bool operator==(const AFGBuildableLightSource* LightSource) const
	{
		return mBuildableLightSource == LightSource;
	}

	FORCEINLINE bool operator==(const FBuildableLightingConnection LightingConnection1) const
	{
		return mBuildableLightSource == LightingConnection1.mBuildableLightSource;
	}

};

USTRUCT(Blueprintable)
struct SMARTWIRELESSLIGHTING_API FSmartLightingBucket
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	TArray<FBuildableLightingConnection> mBuildableLightingConnections;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class ASmartLightsControlPanel* mControlPanel;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGCircuitConnectionComponent* mDownstreamConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	int32 mDirtyIndex;

};

/**
 * 
 */
UCLASS(Blueprintable)
class SMARTWIRELESSLIGHTING_API ASmartLightsControlPanel : public AFGBuildableLightsControlPanel
{
	GENERATED_BODY()

public:
	ASmartLightsControlPanel();

	// Begin Actor Interface
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	// End Actor Interface

	virtual void OnDismantleEffectFinished() override;

	//~ Begin IFGDismantleInterface
	/*virtual void StartIsLookedAtForDismantle_Implementation(class AFGCharacterPlayer* byCharacter) override;
	virtual void GetDismantleRefund_Implementation(TArray< FInventoryStack >& out_refund) const override;
	virtual void PreUpgrade_Implementation() override;*/
	//~ End IFGDismantleInterface

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateControlPanelSmartLightingBucket(FSmartLightingBucket UpdatedSmartLightingBucket);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddBuildableLightSource(class AFGBuildableLightSource* LightSource);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void RemoveBuildableLightSource(class AFGBuildableLightSource* LightSource);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddControlPanelDownstreamConnectionToSmartLightingBucket(class UFGCircuitConnectionComponent* DownstreamConnection);

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightControlData(FLightSourceControlData data);

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightStatus(bool LightStatus);

	UFUNCTION(BlueprintImplementableEvent, Category = "WirelessLightsControlPanel|LightPanel")
	void OnLightingConnectionDestoryed(AActor* destoryedActor);

	/** Get the control panel SmartLightingBucket, valid on client. */
	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingBucket", CompactNodeTitle = "SmartLightingBucket" ))
	FSmartLightingBucket GetControlPanelSmartLightingBucket();

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnections", CompactNodeTitle = "BuildableLightingConnections" ))
	TArray<FBuildableLightingConnection>& GetBuildableLightingConnections();

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetDirtyLightingConnection"))
	FBuildableLightingConnection GetDirtyLightingConnection();
	
	UFUNCTION(BlueprintPure, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnectionIndex", CompactNodeTitle = "ConnectionIndex" ))
	int32 GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection);

protected:
	UFUNCTION()
	void OnRep_ControlPanelLightingBucketUpdated();
	
private:

	UPROPERTY(ReplicatedUsing = "OnRep_ControlPanelLightingBucketUpdated" )
	FSmartLightingBucket mControlPanelSmartLightingBucket;

	UPROPERTY(Replicated)
	bool mIsFirstUpdate = true;
	
	class AFGCircuitSubsystem* mCircuitSubsystem;
	

private:

	AFGBuildableWire* GetControlPanelToLightWire(class UFGPowerConnectionComponent* PowerConnection) 
	{
		TArray< class AFGBuildableWire* >& ConnectedWires = *(new TArray<class AFGBuildableWire*>);
		PowerConnection->GetWires(ConnectedWires);
		for (AFGBuildableWire* Wire : ConnectedWires) {
			UFGCircuitConnectionComponent* OppositeCircuitConnection = Wire->GetOppositeConnection(PowerConnection);
			if (OppositeCircuitConnection && OppositeCircuitConnection == mControlPanelSmartLightingBucket.mDownstreamConnection) {
				return Wire;
			}
		}
		return nullptr;
	}
};
