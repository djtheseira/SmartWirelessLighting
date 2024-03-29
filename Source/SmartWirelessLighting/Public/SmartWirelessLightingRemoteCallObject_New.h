#pragma once
#include "CoreMinimal.h"
#include "FGBuildableSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGRemoteCallObject.h"
#include "SmartLightsControlPanel_New.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Buildables/FGBuildableWire.h"
#include "Player/PlayerCommandSender.h"
#include "SmartWirelessLightingRemoteCallObject_New.generated.h"


UCLASS(Blueprintable)
class SMARTWIRELESSLIGHTING_API USmartWirelessLightingRemoteCallObject_New : public UFGRemoteCallObject
{
	GENERATED_BODY()
public:
	USmartWirelessLightingRemoteCallObject_New();
	
	/** Command Sender associated with this player */
	UPROPERTY(BlueprintReadOnly)
	UPlayerCommandSender* CommandSender;

	// Begin Actor Interface
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	// End Actor Interface

	static USmartWirelessLightingRemoteCallObject_New* getRCO(UWorld* world);

	static USmartWirelessLightingRemoteCallObject_New* getRCO(UObject* worldContext);

	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO2", DisplayName = "Get SWL RCO New")
	static USmartWirelessLightingRemoteCallObject_New* getRCO(AActor* actor) {
		return getRCO(actor->GetWorld());
	}

	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO2")
	void SetBuildableSubsystem();

	UFUNCTION(BlueprintCallable, Category = "SmartWirelessRCO2|Debug")
	void DebugWirelessLightingRCO(FString Location);

	/** 
	* Get all the BuildableLightingSources in the world. This will be used on init for the subsystem itself so that we have
	* all the light sources available without having to constantly query the world.
	*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void FindAllBuildableLightSources(class ASmartLightsControlPanel_New* mControlPanel, const TArray<FBuildableLightingConnection_New>& BuildableLightingConnections);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void SetAvailableLightsForControlPanel(class ASmartLightsControlPanel_New* mControlPanel);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void AddLightConnectionToControlPanel(class ASmartLightsControlPanel_New* ControlPanel, FBuildableLightingConnection_New BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void RemoveLightConnectionFromControlPanel(class ASmartLightsControlPanel_New* ControlPanel, FBuildableLightingConnection_New BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void UpdateControlPanelStatus(class ASmartLightsControlPanel_New* ControlPanel, bool IsEnabled);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void UpdateLightControlData(class ASmartLightsControlPanel_New* ControlPanel, FLightSourceControlData Data);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void UpdateLightColorSlot(class ASmartLightsControlPanel_New* ControlPanel, uint8 slotIdx, FLinearColor NewColor);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "SmartWirelessRCO2|Lights")
	void OnDismantleEffectFinishedDestroyConnections(class ASmartLightsControlPanel_New* ControlPanel);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "SmartWirelessRCO2|Lights")
	TArray<AFGBuildableLightSource*> mBuildableLightSources = *(new TArray<AFGBuildableLightSource*>);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "SmartWirelessRCO2|Lights")
	TArray<FBuildableLightingConnection_New> mBuildableLightingConnections = *(new TArray<FBuildableLightingConnection_New>);

private:
	/** Needed for RCO to work */
	UPROPERTY(Replicated)
	bool dummy;

	class AFGBuildableSubsystem* mBuildableSubsystem;
	
};