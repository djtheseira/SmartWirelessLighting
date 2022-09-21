#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightSource.h"
#include "SmartLightsControlPanel_New.h"
#include "SmartLightsControlPanelSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildableLightSourceListUpdated);

/**
 * 
 */
UCLASS(Blueprintable)
class SMARTWIRELESSLIGHTING_API ASmartLightsControlPanelSubsystem : public AModSubsystem
{
	GENERATED_BODY()

public:
	ASmartLightsControlPanelSubsystem();

	// Begin Actor Interface
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	// End Actor Interface

	static ASmartLightsControlPanelSubsystem* getSubsystem(UWorld* world);

	UFUNCTION(BlueprintCallable, Category = "SmartWirelessLighting2", DisplayName = "Get SWL RCO New")
	static ASmartLightsControlPanelSubsystem* getSubsystem(AActor* actor) {
		return getSubsystem(actor->GetWorld());
	}

	UFUNCTION()
	void RespondToBuildableDismanted();

	UFUNCTION()
	void RespondToBuildableConstructedGlobal(AFGBuildable* buildable);

	UFUNCTION()
	void RespondToLightSourceDestroyed(AActor* DestroyedActor);

	void AddNewLightSource(AFGBuildableLightSource* LightSource);
	void RemoveDestroyedLightSource(const AFGBuildableLightSource* LightSource);
	void UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor);

	void GetAllLightSources();

	UFUNCTION(BlueprintCallable)
	TArray< FBuildableLightingConnection_New> GetControlPanelLightSources(ASmartLightsControlPanel_New* ControlPanel);

	UPROPERTY(BlueprintReadOnly, Category = "SmartWirelessLighting2", DisplayName = "BuildableSubsystem")
	class AFGBuildableSubsystem* mBuildableSubsystem;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SmartWirelessLighting2", DisplayName = "Buildable Light Sources")
	TArray<AFGBuildableLightSource*> mBuildableLightSources = *(new TArray<AFGBuildableLightSource*>);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SmartWirelessLighting2", DisplayName = "Buildable Light Sources")
	TArray<FBuildableLightingConnection_New> mBuildableLightingConnections = *(new TArray<FBuildableLightingConnection_New>);

	UPROPERTY(BlueprintAssignable, Category = "SmartWirelessLighting2")
	FBuildableLightSourceListUpdated OnBuildableLightSourceStateChanged;

	bool mLightListDirty = false;

	AFGBuildableWire* GetControlPanelToLightWire(class UFGPowerConnectionComponent* PowerConnection, class UFGCircuitConnectionComponent* ControlPanelDownstreamConnection)
	{
		TArray< class AFGBuildableWire* >& ConnectedWires = *(new TArray<class AFGBuildableWire*>);
		PowerConnection->GetWires(ConnectedWires);
		for (AFGBuildableWire* Wire : ConnectedWires) {
			UFGCircuitConnectionComponent* OppositeCircuitConnection = Wire->GetOppositeConnection(PowerConnection);
			if (OppositeCircuitConnection && OppositeCircuitConnection == ControlPanelDownstreamConnection) {
				return Wire;
			}
		}
		return nullptr;
	}

	bool IsLightConnectedToLightsControlPanel(class UFGPowerConnectionComponent* PowerConnection)
	{
		bool HasConnectionToControlPanel = false;
		if (PowerConnection->GetPowerCircuit())
		{
			TArray< class UFGCircuitConnectionComponent* >& AllCircuitConnections = *(new TArray<class UFGCircuitConnectionComponent*>);
			AllCircuitConnections = PowerConnection->GetPowerCircuit()->GetComponents();
			if (AllCircuitConnections.Num())
			{
				for (class UFGCircuitConnectionComponent* CircuitConnection : AllCircuitConnections)
				{
					HasConnectionToControlPanel = (CircuitConnection && CircuitConnection->GetOwner() && (CircuitConnection->GetOwner()->GetName().Contains("Build_LightsControlPanel")
						|| CircuitConnection->GetOwner()->GetName().Contains("Build_SmartWirelessLightingControlPanel_New") || CircuitConnection->GetOwner()->GetName().Contains("Build_SmartWirelessLightingControlPanel")));
					if (HasConnectionToControlPanel) break;
				}
			}
		}
		return HasConnectionToControlPanel;
	}

private:

	ELightSourceType_New GetBuildableLightSourceType(FString LightSourceName)
	{
		if (LightSourceName.Contains("Build_StreetLight"))
		{
			return ELightSourceType_New::LS_StreetLight;
		}
		else if (LightSourceName.Contains("Build_CeilingLight"))
		{
			return ELightSourceType_New::LS_CeilingLight;
		}
		else if (LightSourceName.Contains("Build_FloodlightPole"))
		{
			return ELightSourceType_New::LS_PoleFloodLight;
		}
		else
		{
			return ELightSourceType_New::LS_WallFloodLight;
		}
	}


};