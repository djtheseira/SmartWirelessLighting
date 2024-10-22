#include "SmartLightsControlPanel.h"

#include "Build_SmartWirelessWireBase.h"
#include "SmartLightsControlPanelSubsystem.h"

#include "FGPlayerController.h"
#include "FGCircuitConnectionComponent.h"
#include "FGCircuitSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGInventoryComponent.h"
#include "FGUseableInterface.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerCircuit.h"
#include "FGPowerInfoComponent.h"
#include "Logging.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ASmartLightsControlPanel::ASmartLightsControlPanel() : Super()
{
	this->bReplicates = true;
}

void ASmartLightsControlPanel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmartLightsControlPanel, mBuildableLightingConnections);
	DOREPLIFETIME(ASmartLightsControlPanel, mAvailableLightSources);
	DOREPLIFETIME(ASmartLightsControlPanel, mSmartPanelDownstreamConnection);
	DOREPLIFETIME(ASmartLightsControlPanel, mBuildableLightConnectionCount);
}

void ASmartLightsControlPanel::BeginPlay()
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::BeginPlay"));
	Super::BeginPlay();
	AFGCircuitSubsystem* CircuitSubsystem = AFGCircuitSubsystem::Get(GetWorld());
	mSmartPanelDownstreamConnection = Super::mDownstreamConnection;

	if (CircuitSubsystem) {
		mCircuitSubsystem = CircuitSubsystem;
	}
}

void ASmartLightsControlPanel::AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel"));
	BuildableLightingConnection.mBuildablePowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
	ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
	int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, mSmartPanelDownstreamConnection);
	mSmartPanelDownstreamConnection->AddHiddenConnection(BuildableLightingConnection.mBuildablePowerConnection);

	BuildableLightingConnection.mBuildableWire = NewBuildableWire;
	BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(this->IsLightEnabled());
	BuildableLightingConnection.mBuildableLightSource->SetLightControlData(this->GetLightControlData());
	BuildableLightingConnection.isConnected = true;
	BuildableLightingConnection.mShouldShow = true;
	mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
}

void ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection, class ASmartLightsControlPanelSubsystem* SmartLightsControlPanelSubsystem) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel"));
	BuildableLightingConnection.mBuildablePowerConnection = mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource);
	if (!BuildableLightingConnection.mBuildableWire)
	{
		AFGBuildableWire* BuildableWire = mSmartLightsControlPanelSubsystem->GetControlPanelToLightWire(BuildableLightingConnection.mBuildablePowerConnection, mSmartPanelDownstreamConnection);
		if (BuildableWire)
		{
			BuildableLightingConnection.mBuildableWire = BuildableWire;
		}
	}

	if (BuildableLightingConnection.mBuildableWire) {
		BuildableLightingConnection.mBuildableWire->Disconnect();
		BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
		BuildableLightingConnection.mBuildableWire->Destroy(true);
	}

	mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, (mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)));
	mSmartPanelDownstreamConnection->RemoveHiddenConnection(BuildableLightingConnection.mBuildablePowerConnection);

	int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
	BuildableLightingConnection.isConnected = false;
	BuildableLightingConnection.mShouldShow = true;
	BuildableLightingConnection.mBuildableWire = nullptr;
	mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
}

TArray< FBuildableLightingConnection> ASmartLightsControlPanel::GetBuildableLightingConnections(ELightSourceType LightSourceType) {

	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::mSmartLightsControlPanelSubsystem->mLightListDirty: %s"), mSmartLightsControlPanelSubsystem->mLightListDirty ? TEXT("true"): TEXT("false"));

	TArray<FBuildableLightingConnection> filteredLightingConnections = mBuildableLightingConnections.FilterByPredicate([&LightSourceType](FBuildableLightingConnection BuildableLightingConnection) {
		/*if (BuildableLightingConnection.mBuildableLightSource) {
			UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections BuildableLightingConnection.mBuildableLightSource Intensity: %f"), BuildableLightingConnection.mBuildableLightSource->GetLightControlData().Intensity);
		}*/
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections actual LightSourceType: %s"), *UEnum::GetValueAsString(LightSourceType));
		if (LightSourceType  == ELightSourceType::LS_WallFloodLight && 
				(BuildableLightingConnection.mLightSourceType == ELightSourceType::LS_WallFloodLight 
					|| BuildableLightingConnection.mLightSourceType == ELightSourceType::LS_PoleFloodLight)) {
			return true;
		}
		else {
			return BuildableLightingConnection.mLightSourceType == LightSourceType;
		}
	});
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections mBuildableLightingConnections %d"), filteredLightingConnections.Num());
	return filteredLightingConnections;
}

void ASmartLightsControlPanel::UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelStatus"));
	Super::SetLightEnabled(IsEnabled);

	TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus LightSourcesNum: %d"), BuildableLightSources.Num());

	for (AFGBuildable* Buildable : BuildableLightSources) {
		AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
		if (lightSource) {
			lightSource->SetLightEnabled(IsEnabled);
		}
	}
}

void ASmartLightsControlPanel::UpdateLightControlData(FLightSourceControlData Data) {
	//UE_LOG(LogSmartWirelessLighting, Verbose, TEXT(".ASmartLightsControlPanel::UpdateLightControlData"));
	 Super::SetLightDataOnControlledLights(Data);
}

void ASmartLightsControlPanel::RespondToLightColorSlotUpdate() {
	
}

///*
// * Keeping this here for my sake..gonna try to implmement this at some point..at this point, just dont feel like working on this bug.
// * Refreshing the list isn't that intensive, at least not yet..
// */
////void ASmartLightsControlPanel::BindTo_OnLightConnectionChanged(class UFGCircuitConnectionComponent* CircuitConnection)
////{
////	UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::BindTo_OnLightConnectionChanged %s %d"), HasAuthority() ? TEXT("Auth") : TEXT("Remote"), CircuitConnection->GetCircuitID());
////	if (HasAuthority()) {
////		CircuitConnection->OnConnectionChanged.RemoveAll(this);
////		RefreshControlPanelBucket();
////	}
////}

void ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections()
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections %s size %d "), HasAuthority() ? TEXT("auth") : TEXT("remote"), mBuildableLightingConnections.Num());
	//mIsDirtyList = true;
}