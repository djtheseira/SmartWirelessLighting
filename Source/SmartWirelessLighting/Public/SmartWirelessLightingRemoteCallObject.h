#pragma once
#include "CoreMinimal.h"
#include "FGBuildableSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGRemoteCallObject.h"
#include "SmartLightsControlPanel.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Buildables/FGBuildableWire.h"
#include "Player/PlayerCommandSender.h"
#include "SmartWirelessLightingRemoteCallObject.generated.h"


UCLASS(Blueprintable)
class SMARTWIRELESSLIGHTING_API USmartWirelessLightingRemoteCallObject : public UFGRemoteCallObject
{
	GENERATED_BODY()
public:
	USmartWirelessLightingRemoteCallObject();
	
	/** Command Sender associated with this player */
	UPROPERTY(BlueprintReadOnly)
	UPlayerCommandSender* CommandSender;

	// Begin Actor Interface
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	// End Actor Interface

	/** 
	* Get all the BuildableLightingSources in the world. This will be used on init for the subsystem itself so that we have
	* all the light sources available without having to constantly query the world.
	*/
	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO|Lights")
	void FindAllBuildableLightSources(TArray<FBuildableLightingConnection>& BuildableLightingConnections);

	/** 
	* Get the available light sources in the world, weird name, but basically this updates the Buildable Lighting Connection objects with the information
	* about power status for the lights when rendering them in the list.
	*/
	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO|Lights")
	void UpdateLightControlPanelBuildableLightSources(UPARAM(Ref)FSmartLightingBucket& SmartLightingBucket);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void Server_UpdateLightControlPanelBucket(class ASmartLightsControlPanel* mControlPanel, FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void Server_RefreshLightControlPanelBucket(class ASmartLightsControlPanel* mControlPanel);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void AddLightConnection(FBuildableLightingConnection BuildableLightingConnection, class ASmartLightsControlPanel* mControlPanel);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void RemoveLightConnection(FBuildableLightingConnection BuildableLightingConnection, class ASmartLightsControlPanel* mControlPanel);

	/** Update the Light Source's Light Control Data */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void UpdateLightControlData(class ASmartLightsControlPanel* mControlPanel, FLightSourceControlData LightControlData);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Lights")
	void UpdateControlPanelStatus(class ASmartLightsControlPanel* mControlPanel, bool IsEnabled);

	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO|Debug")
	void DebugWirelessLightingRCO(FString Location);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO|Colors")
	void Server_UpdateLightColorSlot(uint8 slotIdx, FLinearColor color);

	UFUNCTION(BlueprintGetter, Category = "SmartWirelessRCO|Lights")
	bool IsLightSourceConnectedToControlPanelConnection(UFGCircuitConnectionComponent* LightConnection, UFGCircuitConnectionComponent* DownstreamConnection) const;
	
private:
	/** Needed for RCO to work */
	UPROPERTY()
	int32 DummyReplicatedField;

	UPROPERTY()
	TArray<FBuildableLightingConnection> mBuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);

	UPROPERTY()
	TArray<AFGBuildableLightSource*> mBuildableLightSources = *(new TArray<AFGBuildableLightSource*>);

	UPROPERTY()
	bool mIsLightListDirty;

	class AFGBuildableSubsystem* mBuildableSubsytem;
	
};