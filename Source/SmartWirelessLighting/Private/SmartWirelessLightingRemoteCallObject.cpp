#include "SmartWirelessLightingRemoteCallObject.h"
#include "Build_SmartWirelessWireBase.h"
#include "FGBuildableSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FGRemoteCallObject.h"
#include "FGWorldSettings.h"
#include "Logging.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "GameFramework/Actor.h"
#include "Hologram/FGWireHologram.h"
#include "Net/UnrealNetwork.h"
#include "Registry/RemoteCallObjectRegistry.h"


USmartWirelessLightingRemoteCallObject::USmartWirelessLightingRemoteCallObject() : Super()
{
	CommandSender = CreateDefaultSubobject<UPlayerCommandSender>(TEXT("PlayerCommandSender"));
}

void USmartWirelessLightingRemoteCallObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, DummyReplicatedField);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, mBuildableLightSources);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, mBuildableLightingConnections);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, mIsLightListDirty);
}

void USmartWirelessLightingRemoteCallObject::FindAllBuildableLightSources(TArray< FBuildableLightingConnection>& BuildableLightingConnections)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::FindAllBuildableLightSources"));
	TArray< AFGBuildableLightSource* >& Buildables = *(new TArray<AFGBuildableLightSource*>);
	mBuildableSubsytem = Cast<AFGBuildableSubsystem>(Cast<AFGWorldSettings>(GetWorld()->GetWorldSettings(false, false))->GetBuildableSubsystem());
	mBuildableSubsytem->GetTypedBuildable(Buildables);
	BuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);
	for (AFGBuildable* buildable : Buildables) {
		AFGBuildableLightSource* LightSource = Cast<AFGBuildableLightSource>(buildable);
		if (LightSource) {
			UFGPowerConnectionComponent* LightConnection = Cast<UFGPowerConnectionComponent>(LightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
			FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
			LightingConnection.mBuildableLightSource = LightSource;
			BuildableLightingConnections.Add(LightingConnection);
		}
	}
}

void USmartWirelessLightingRemoteCallObject::UpdateLightControlPanelBuildableLightSources(FSmartLightingBucket& SmartLightingBucket) 
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateWorldBuildableLightSources"));

	for (FBuildableLightingConnection LightingConnection : SmartLightingBucket.mBuildableLightingConnections) {
		AFGBuildableLightSource* lightSource = LightingConnection.mBuildableLightSource;
		if (lightSource) {			
			UFGPowerConnectionComponent* LightConnection = Cast<UFGPowerConnectionComponent>(lightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
			bool HasHiddenConnection = false;
			if (LightConnection) {
				if (LightConnection->GetCircuitID() > -1 && !IsLightSourceConnectedToControlPanelConnection(LightConnection, SmartLightingBucket.mDownstreamConnection)) {
					SmartLightingBucket.mBuildableLightingConnections.RemoveAt(SmartLightingBucket.mBuildableLightingConnections.IndexOfByKey(lightSource));
				}
			}
		}
	}
}

void USmartWirelessLightingRemoteCallObject::AddLightConnection_Implementation(FBuildableLightingConnection BuildableLightingConnection, FSmartLightingBucket SmartLightingBucket)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::AddLightConnection"));
	SmartLightingBucket.mControlPanel->AddLightingConnectionToControlPanel(BuildableLightingConnection);
}

void USmartWirelessLightingRemoteCallObject::RemoveLightConnection_Implementation(FBuildableLightingConnection BuildableLightingConnection, FSmartLightingBucket SmartLightingBucket)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::RemoveLightConnection"));
	SmartLightingBucket.mControlPanel->RemoveLightingConnectionToControlPanel(BuildableLightingConnection);
}

void USmartWirelessLightingRemoteCallObject::UpdateLightControlData_Implementation(FSmartLightingBucket SmartLightingBucket, FLightSourceControlData LightControlData)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateLightControlData"));
	SmartLightingBucket.mControlPanel->UpdateLightControlData(LightControlData);
}

void USmartWirelessLightingRemoteCallObject::UpdateControlPanelStatus_Implementation(FSmartLightingBucket SmartLightingBucket, bool IsEnabled)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateControlPanelStatus"));
	SmartLightingBucket.mControlPanel->UpdateLightStatus(IsEnabled);
}

void USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO(FString Location)
{
	UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO %s"), *Location);
}

void USmartWirelessLightingRemoteCallObject::Server_UpdateLightColorSlot_Implementation(uint8 slotIdx, FLinearColor color)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::Server_UpdateLightColorSlot"));
	class AFGGameState* FactoryGameState = Cast<AFGGameState>(GetWorld()->GetGameState());
	if (FactoryGameState) {
		FactoryGameState->Server_SetBuildableLightColorSlot(slotIdx, color);
	}
}

bool USmartWirelessLightingRemoteCallObject::IsLightSourceConnectedToControlPanelConnection(UFGCircuitConnectionComponent* LightConnection, UFGCircuitConnectionComponent* DownstreamConnection) const
{
	TArray< UFGCircuitConnectionComponent* >& LightConnections = *(new TArray< UFGCircuitConnectionComponent*>);
	LightConnection->GetConnections(LightConnections);
	bool IsConnected = false;
	for (UFGCircuitConnectionComponent* Connection : LightConnections) {
		if (Connection) {
			IsConnected = Connection == DownstreamConnection;
		}

		if (IsConnected) {
			break;
		}
	}
	return IsConnected;
}

