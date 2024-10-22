#pragma once

#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGPowerCircuit.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "Buildables/FGBuildableLightSource.h"
#include "SmartLightsControlPanel.h"
#include "SmartLightsControlPanelSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildableLightSourceListUpdated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildableLightSourceChanged, class ASmartLightsControlPanel*, controlPanel);

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

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem", DisplayName = "Get SWL RCO New")
	static ASmartLightsControlPanelSubsystem* getSubsystem(AActor* actor) {
		return getSubsystem(actor->GetWorld());
	}

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	void InitializeAvailableLightList();

	UFUNCTION(BlueprintCallable, Category="SWL|Subsystem")
	TArray<FBuildableLightingConnection> GetAvailableLightListForControlPanel(ASmartLightsControlPanel* ControlPanel);

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	void AddNewLightSource(AFGBuildableLightSource* LightSource);

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	void RemoveDestroyedLightSource(AFGBuildableLightSource* LightSource);

	UFUNCTION()
	void RespondToPanelDataChanged(bool isEnabled);

	UFUNCTION()
	void RespondToLightSourceDestroyed(AActor* DestroyedActor);

	void UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor);
	void OnControlPanelToLightConnectionUpdate(class ASmartLightsControlPanel* controlPanel);
	void OnSmartWirelessLightControlPanelDestroyed();

	void GetAllLightSources();

	UFUNCTION()
	void RespondToBuildableConstructedGlobal(AFGBuildable* buildable);

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	TArray< FBuildableLightingConnection> GetControlPanelLightSources(ASmartLightsControlPanel* ControlPanel);

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	TArray< class UFGCircuitConnectionComponent* >GetAllCircuitConnections(UFGPowerConnectionComponent* LightPowerConnection);

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
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

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	bool IsLightPowerCircuitToLightsControlPanel(class UFGPowerCircuit* PowerCircuit);

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
						|| CircuitConnection->GetOwner()->GetName().Contains("Build_SmartWirelessLightingControlPanel")
						|| CircuitConnection->GetOwner()->GetName().Contains("Build_SmartWirelessLightingControlPanel_New")));
					if (HasConnectionToControlPanel) break;
				}
			}
		}
		return HasConnectionToControlPanel;
	}

	UFUNCTION(BlueprintCallable, Category = "SWL|Subsystem")
	UFGPowerConnectionComponent* GetLightSourcePowerConnectionComponent(AFGBuildableLightSource* BuildableLightSource) {

		if (!BuildableLightSource) return nullptr;
		UFGPowerConnectionComponent* PowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));

		if (!PowerConnection) return nullptr;
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::GetLightSourcePowerConnectionComponent %s LightPowerConnection %s"), *(BuildableLightSource->GetName()), (PowerConnection ? TEXT("Has PowerConnection") : TEXT("No PowerConnection")));
		return PowerConnection;


		return nullptr;
	}

	UPROPERTY(BlueprintReadOnly, Category = "SWL|Subsystem", DisplayName = "BuildableSubsystem")
	class AFGBuildableSubsystem* mBuildableSubsystem;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SWL|Subsystem", DisplayName = "Buildable Light Sources")
	TArray<AFGBuildableLightSource*> mBuildableLightSources = *(new TArray<AFGBuildableLightSource*>);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SWL|Subsystem", DisplayName = "Buildable Lighting Connections")
	TArray<FBuildableLightingConnection> mBuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);

	UPROPERTY(BlueprintAssignable, Category = "SWL|Subsystem")
	FBuildableLightSourceListUpdated OnBuildableLightSourceStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "SWL|Subsystem")
	FBuildableLightSourceChanged OnLightSourceStateChanged;

	// Probably Deprecated
	bool mLightListDirty = false;

	// Probably Deprecated
	void SetLightListIsDirty(bool isLightListDirty);

private:

	//FBuildableLightingConnection CreateBuildableLightingConnection(AFGBuildableLightSource* BuildableLightSource);

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
