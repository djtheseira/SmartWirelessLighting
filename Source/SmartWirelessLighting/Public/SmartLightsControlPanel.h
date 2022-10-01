#pragma once

#include "CoreMinimal.h"
#include "FactoryGame.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGCircuitSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGColoredInstanceMeshProxy.h"
#include "FGInventoryComponent.h"
#include "FGPowerCircuit.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FGUseableInterface.h"
#include "Logging.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableCircuitBridge.h"
#include "Buildables/FGBuildableControlPanelHost.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Buildables/FGBuildableLightSource.h"
#include "SmartLightsControlPanel.generated.h"

UENUM(BlueprintType)
enum class ELightSourceType : uint8
{
	LS_StreetLight UMETA(DisplayName = "Street Light"),
	LS_CeilingLight UMETA(DisplayName = "Ceiling Light"),
	LS_WallFloodLight UMETA(DisplayName = "Wall Flood Light"),
	LS_PoleFloodLight UMETA(DisplayName = "Pole Flood Light")
};

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

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	ELightSourceType mLightSourceType;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	float mDistanceToControlPanel = 0;

	FORCEINLINE bool operator==(const AFGBuildableLightSource* LightSource) const
	{
		return mBuildableLightSource == LightSource;
	}

	FORCEINLINE bool operator==(const FBuildableLightingConnection LightingConnection1) const
	{
		return mBuildableLightSource == LightingConnection1.mBuildableLightSource;
	}
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

	//~ Begin IFGDismantleInterface
	virtual void OnDismantleEffectFinished() override;
	void Server_OnDismantleEffectFinished();
	//~ End IFGDismantleInterface

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void SetAvailableLightsForControlPanel();
	void Server_SetAvailableLightsForControlPanel();

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);
	void Server_AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection);
	void Server_RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void UpdateControlPanelStatus(bool IsEnabled);
	void Server_UpdateControlPanelStatus(bool IsEnabled);

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void UpdateLightControlData(FLightSourceControlData Data);
	void Server_UpdateLightControlData(FLightSourceControlData Data);

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel|LightPanel")
	void UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor);
	void Server_UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor);

	UFUNCTION()
	void RespondToBuildableLightSourceListUpdated();

	UFUNCTION()
	void RespondToLightSourceStateChange(class ASmartLightsControlPanel* controlPanel);
	
	UFUNCTION()
	void RespondToLightColorSlotUpdate();

	void SetBuildableLightConnectionCount(uint8 BuildableLightConnectionCount);

	UFUNCTION(BlueprintCallable, Category = "New_WirelessLightsControlPanel", DisplayName = "GetBuildableLightingConnections")
	TArray< FBuildableLightingConnection> GetBuildableLightingConnections(ELightSourceType LightSourceType);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SmartWirelessLighting2", DisplayName = "Buildable Light Sources")
	TArray<AFGBuildableLightSource*> mAvailableLightSources = *(new TArray<AFGBuildableLightSource*>);

	UPROPERTY(Replicated)
	class UFGCircuitConnectionComponent* mSmartPanelDownstreamConnection;

	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_ControlPanelBuildableLightingConnections")
	TArray<FBuildableLightingConnection> mBuildableLightingConnections;

	uint8 mBuildableLightConnectionCount = 0;

protected:

	UFUNCTION()
	void OnRep_ControlPanelBuildableLightingConnections();

private:

	UPROPERTY(Replicated)
	bool mIsFirstUpdate = true;

	class AFGCircuitSubsystem* mCircuitSubsystem;

	class ASmartLightsControlPanelSubsystem* mSmartLightsControlPanelSubsystem;

	ELightSourceType GetBuildableLightSourceType(FString LightSourceName)
	{
		if (LightSourceName.Contains("Build_StreetLight"))
		{
			return ELightSourceType::LS_StreetLight;
		}
		else if (LightSourceName.Contains("Build_CeilingLight"))
		{
			return ELightSourceType::LS_CeilingLight;
		}
		else if (LightSourceName.Contains("Build_FloodlightPole"))
		{
			return ELightSourceType::LS_PoleFloodLight;
		}
		else
		{
			return ELightSourceType::LS_WallFloodLight;
		}
	}

};
