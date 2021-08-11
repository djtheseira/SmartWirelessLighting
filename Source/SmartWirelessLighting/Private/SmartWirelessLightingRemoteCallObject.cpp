#include "SmartWirelessLightingRemoteCallObject.h"
#include "FGBuildableSubsystem.h"
#include "FGGameState.h"
#include "FGRemoteCallObject.h"
#include "Buildables/FGBuildableWire.h"
#include "Build_SmartWirelessWireBase.h"
#include "Buildables/FGBuildable.h"
#include "Hologram/FGWireHologram.h"
#include "FGPlayerController.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "Net/UnrealNetwork.h"
#include "Registry/RemoteCallObjectRegistry.h"
#include "FGGameState.h"
#include "GameFramework/Actor.h"
#include "FGWorldSettings.h"

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
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::FindAllBuildableLightSources"));
	TArray< AFGBuildableLightSource* >& Buildables = *(new TArray<AFGBuildableLightSource*>);
	if (mBuildableSubsytem == nullptr) {
		mBuildableSubsytem = Cast<AFGBuildableSubsystem>(Cast<AFGWorldSettings>(GetWorld()->GetWorldSettings(false, false))->GetBuildableSubsystem());
	}
	mBuildableSubsytem->GetTypedBuildable(Buildables);
	BuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);
	for (AFGBuildable* buildable : Buildables) {
		AFGBuildableLightSource* LightSource = Cast<AFGBuildableLightSource>(buildable);
		if (LightSource) {
			UFGCircuitConnectionComponent* LightConnection = Cast<UFGCircuitConnectionComponent>(LightSource->GetComponentByClass(UFGCircuitConnectionComponent::StaticClass()));
			FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
			LightingConnection.mBuildableLightSource = LightSource;
			BuildableLightingConnections.Add(LightingConnection);
		}
	}
}

void USmartWirelessLightingRemoteCallObject::AddNewBuiltBuildableLightSource(AFGBuildableLightSource* LightSource, FSmartLightingBucket& SmartLightingBucket)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::AddNewBuiltBuildableLightSource"));
	if (LightSource) {
		FBuildableLightingConnection lightingConnection = FBuildableLightingConnection();
		lightingConnection.mBuildableLightSource = LightSource;
		SmartLightingBucket.mBuildableLightingConnections.Add(lightingConnection);
	}
}

void USmartWirelessLightingRemoteCallObject::RemoveBuiltBuildableLightSource(AFGBuildableLightSource* LightSource, FSmartLightingBucket& SmartLightingBucket)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::RemoveBuiltBuildableLightSource"));
	if (LightSource) {

		int32 indexKey = SmartLightingBucket.mBuildableLightingConnections.IndexOfByKey(LightSource);
		if (indexKey != INDEX_NONE) {
			SmartLightingBucket.mBuildableLightingConnections.RemoveAt(indexKey);
		}
	}
}

void USmartWirelessLightingRemoteCallObject::UpdateLightControlPanelBuildableLightSources(FSmartLightingBucket& SmartLightingBucket) 
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateWorldBuildableLightSources"));

	for (FBuildableLightingConnection LightingConnection : SmartLightingBucket.mBuildableLightingConnections) {
		AFGBuildableLightSource* lightSource = LightingConnection.mBuildableLightSource;
		if (lightSource) {			
			UFGCircuitConnectionComponent* LightConnection = Cast<UFGCircuitConnectionComponent>(lightSource->GetComponentByClass(UFGCircuitConnectionComponent::StaticClass()));
			bool HasHiddenConnection = false;
			if (LightConnection) {
				if (LightConnection->GetCircuitID() > -1 && !IsLightSourceConnectedToControlPanelConnection(LightConnection, SmartLightingBucket.mDownstreamConnection)) {
					SmartLightingBucket.mBuildableLightingConnections.RemoveAt(SmartLightingBucket.mBuildableLightingConnections.IndexOfByKey(lightSource));
				}
			}
		}
	}
}

/*
void USmartWirelessLightingRemoteCallObject::UpdateLightConnection_Implementation(AFGBuildableLightSource* LightSource, FSmartLightingBucket SmartLightingBucket, class UFGPowerConnectionComponent* LightSourceConnection, class AFGBuildableWire* BuildableWire, const bool IsConnected)
{
	
	UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateLightConnection %s"), (IsConnected ? TEXT("Connecting") : TEXT("Disconnecting")));
	FBuildableLightingConnection* BuildableLightConnection = mBuildableLightingConnections.FindByKey(LightSource);
	//int32 BuildableIndex = mBuildableLightingConnections.Find()
	if (IsConnected) {
		BuildableWire->Connect(SmartLightingBucket.mDownstreamConnection, LightSourceConnection);
		BuildableLightConnection->mBuildableWire = BuildableWire;
		BuildableLightConnection->mDownstreamConnection = SmartLightingBucket.mDownstreamConnection;
		//mBuildableLightingConnections[BuildableLightConnection] = BuildableLightConnection;
		//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateLightConnection Connecting %s"), (mBuildableLightingConnections.FindByKey(LightSource)->isConnected ? TEXT("iscon") : TEXT("isnotcon")));
	}
	else {
		TArray< AFGBuildableWire* >& BuildableWires = *(new TArray<AFGBuildableWire*>);
		LightSourceConnection->GetWires(BuildableWires);
		int32 IndexOfWire = BuildableWires.Find(BuildableLightConnection->mBuildableWire);
		if (IndexOfWire != INDEX_NONE) {
			BuildableWires[IndexOfWire]->TurnOffAndDestroy();
			BuildableWires[IndexOfWire]->Destroy(true);
		}
		BuildableLightConnection->mBuildableWire = nullptr;

	}
	LightSource->SetLightEnabled(IsConnected && ControlPanel->IsLightEnabled());
	LightSource->SetLightControlData(ControlPanel->GetLightControlData());
	BuildableLightConnection->isConnected = IsConnected;
	mIsLightListDirty = true;
	OnBuildableLightingConnectionsChanged.Broadcast(DownstreamConnection);

}
*/

void USmartWirelessLightingRemoteCallObject::AddLightConnection_Implementation(FBuildableLightingConnection BuildableLightingConnection, FSmartLightingBucket SmartLightingBucket)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::AddLightConnection"));
	SmartLightingBucket.mControlPanel->AddLightingConnectionToControlPanel(BuildableLightingConnection);
}

void USmartWirelessLightingRemoteCallObject::RemoveLightConnection_Implementation(FBuildableLightingConnection BuildableLightingConnection, FSmartLightingBucket SmartLightingBucket)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::RemoveLightConnection"));
	SmartLightingBucket.mControlPanel->RemoveLightingConnectionToControlPanel(BuildableLightingConnection);
}

void USmartWirelessLightingRemoteCallObject::UpdateLightControlData_Implementation(FSmartLightingBucket SmartLightingBucket, FLightSourceControlData LightControlData)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateLightControlData"));
	SmartLightingBucket.mControlPanel->UpdateLightControlData(LightControlData);
}

void USmartWirelessLightingRemoteCallObject::UpdateControlPanelStatus_Implementation(FSmartLightingBucket SmartLightingBucket, bool IsEnabled)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::UpdateControlPanelStatus"));
	SmartLightingBucket.mControlPanel->UpdateLightStatus(IsEnabled);
}

void USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO(FString Location)
{
	UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO %s"), *Location);
}

void USmartWirelessLightingRemoteCallObject::Server_UpdateLightColorSlot_Implementation(uint8 slotIdx, FLinearColor color)
{
	//UE_LOG(LogTemp, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::Server_UpdateLightColorSlot"));
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

