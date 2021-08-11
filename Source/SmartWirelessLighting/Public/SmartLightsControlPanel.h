#pragma once

#include "CoreMinimal.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGUseableInterface.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
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
	//virtual void BeginPlay() override;
	// End Actor Interface
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateControlPanelSmartLightingBucket(FSmartLightingBucket UpdatedSmartLightingBucket);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddControlPanelDownstreamConnectionToSmartLightingBucket(class UFGCircuitConnectionComponent* DownstreamConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void Server_AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void Server_RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightControlData(FLightSourceControlData data);

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightStatus(bool LightStatus);

	/** Get the control panel SmartLightingBucket, valid on client. */
	UFUNCTION(BlueprintPure, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingBucket", CompactNodeTitle = "SmartLightingBucket" ))
	FSmartLightingBucket GetControlPanelSmartLightingBucket() const;

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnections", CompactNodeTitle = "BuildableLightingConnections" ))
	TArray<FBuildableLightingConnection>& GetBuildableLightingConnections();

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetDirtyLightingConnection"))
	FBuildableLightingConnection GetDirtyLightingConnection();
	
	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnectionIndex", CompactNodeTitle = "ConnectionIndex" ))
	int32 GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection);

protected:
	UFUNCTION()
	void OnRep_ControlPanelLightingBucketUpdated();
	
private:

	UPROPERTY(ReplicatedUsing = "OnRep_ControlPanelLightingBucketUpdated" )
	FSmartLightingBucket mControlPanelSmartLightingBucket;
	
};
