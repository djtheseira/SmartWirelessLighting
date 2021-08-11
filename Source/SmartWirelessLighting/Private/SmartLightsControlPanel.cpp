#include "SmartLightsControlPanel.h"
#include "Build_SmartWirelessWireBase.h"
#include "FGCharacterPlayer.h"
#include "FGUseableInterface.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
#include "Buildables/FGBuildable.h"
#include "Net/UnrealNetwork.h"

ASmartLightsControlPanel::ASmartLightsControlPanel() : Super() {
}

void ASmartLightsControlPanel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmartLightsControlPanel, mControlPanelSmartLightingBucket);
}

void ASmartLightsControlPanel::UpdateControlPanelSmartLightingBucket_Implementation(FSmartLightingBucket UpdatedSmartLightingBucket)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelSmartLightingBucket"));
	this->mControlPanelSmartLightingBucket = UpdatedSmartLightingBucket;
}

void ASmartLightsControlPanel::AddControlPanelDownstreamConnectionToSmartLightingBucket_Implementation(class UFGCircuitConnectionComponent* DownstreamConnection)
{
	mControlPanelSmartLightingBucket.mDownstreamConnection = DownstreamConnection;
}

void ASmartLightsControlPanel::Server_AddLightingConnectionToControlPanel_Implementation(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightingConnectionToControlPanel"));
	AddLightingConnectionToControlPanel(BuildableLightingConnection);
}

void ASmartLightsControlPanel::AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel"));
	if (HasAuthority()) 
	{
		ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
		int32 BuildableConnectionIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
		NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, mControlPanelSmartLightingBucket.mDownstreamConnection);
		BuildableLightingConnection.mBuildableWire = NewBuildableWire;
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(mControlPanelSmartLightingBucket.mControlPanel->IsLightEnabled());
		BuildableLightingConnection.mBuildableLightSource->SetLightControlData(mControlPanelSmartLightingBucket.mControlPanel->GetLightControlData());
		BuildableLightingConnection.isConnected = true;
		mControlPanelSmartLightingBucket.mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
		mControlPanelSmartLightingBucket.mDirtyIndex = BuildableConnectionIndex;
	}
}

void ASmartLightsControlPanel::Server_RemoveLightingConnectionToControlPanel_Implementation(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightingConnectionToControlPanel"));
	RemoveLightingConnectionToControlPanel(BuildableLightingConnection);
}

void ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel"));
	
	if (HasAuthority())
	{
		int32 BuildableConnectionIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
		class UFGCircuitConnectionComponent* DownstreamConnection = BuildableLightingConnection.mDownstreamConnection;
		if (BuildableLightingConnection.mBuildableWire) {
			BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
			BuildableLightingConnection.mBuildableWire->Destroy(true);
		}
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
		BuildableLightingConnection.isConnected = false;
		BuildableLightingConnection.mDownstreamConnection = nullptr;
		BuildableLightingConnection.mBuildableWire = nullptr;
		mControlPanelSmartLightingBucket.mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
	}
}

void ASmartLightsControlPanel::UpdateLightControlData_Implementation(FLightSourceControlData data)
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData"));
	mControlPanelSmartLightingBucket.mControlPanel->SetLightControlData(data);
	for (FBuildableLightingConnection BuildableLightingConnection : mControlPanelSmartLightingBucket.mBuildableLightingConnections) {
		BuildableLightingConnection.mBuildableLightSource->SetLightControlData(data);
	}
}

void ASmartLightsControlPanel::UpdateLightStatus_Implementation(bool LightStatus)
{
	mControlPanelSmartLightingBucket.mControlPanel->SetLightEnabled(LightStatus);
	for (FBuildableLightingConnection BuildableLightingConnection : mControlPanelSmartLightingBucket.mBuildableLightingConnections) {
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(LightStatus);
	}
}

FSmartLightingBucket ASmartLightsControlPanel::GetControlPanelSmartLightingBucket() const
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::GetControlPanelSmartLightingBucket"));
	return mControlPanelSmartLightingBucket;
}

int32 ASmartLightsControlPanel::GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection)
{
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
}
TArray<FBuildableLightingConnection>& ASmartLightsControlPanel::GetBuildableLightingConnections()
{
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections;
}

FBuildableLightingConnection ASmartLightsControlPanel::GetDirtyLightingConnection()
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::GetDirtyLightingConnection"));
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections[mControlPanelSmartLightingBucket.mDirtyIndex];
}

void ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated()
{
	//UE_LOG(LogTemp, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated"));
}
