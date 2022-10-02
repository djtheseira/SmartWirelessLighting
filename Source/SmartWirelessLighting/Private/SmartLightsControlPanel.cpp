#include "SmartLightsControlPanel.h"

#include "Build_SmartWirelessWireBase.h"
#include "SmartLightsControlPanelSubsystem.h"
#include "SmartWirelessLightingRemoteCallObject.h"

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
}

void ASmartLightsControlPanel::OnDismantleEffectFinished()
{
	Super::OnDismantleEffectFinished();
	if (HasAuthority()) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished"));
		for (FBuildableLightingConnection LightingConnection : this->mBuildableLightingConnections)
		{
			if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
			{
				LightingConnection.mBuildableWire->Destroy(true);
				LightingConnection.mBuildableWire->TurnOffAndDestroy();
			}
		}
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->OnDismantleEffectFinishedDestroyConnections(this);
	}
}

void ASmartLightsControlPanel::Server_OnDismantleEffectFinished()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_OnDismantleEffectFinished"));
	for (FBuildableLightingConnection LightingConnection : this->mBuildableLightingConnections)
	{
		if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
		{
			LightingConnection.mBuildableWire->Destroy(true);
			LightingConnection.mBuildableWire->TurnOffAndDestroy();
		}
	}

}

void ASmartLightsControlPanel::BeginPlay()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::BeginPlay"));
	Super::BeginPlay();
	AFGCircuitSubsystem* CircuitSubsystem = AFGCircuitSubsystem::Get(GetWorld());
	mSmartPanelDownstreamConnection = Super::mDownstreamConnection;

	//if (!HasAuthority()) return;

	if (CircuitSubsystem) {
		mCircuitSubsystem = CircuitSubsystem;
	}

	ASmartLightsControlPanelSubsystem* SmartLightsControlPanelSubsystem = ASmartLightsControlPanelSubsystem::getSubsystem(GetWorld());

	if (SmartLightsControlPanelSubsystem) {
		mSmartLightsControlPanelSubsystem = SmartLightsControlPanelSubsystem;
		if (HasAuthority()) {

			mSmartLightsControlPanelSubsystem->GetAllLightSources();
			this->SetAvailableLightsForControlPanel();
			mSmartLightsControlPanelSubsystem->OnBuildableLightSourceStateChanged.AddDynamic(this, &ASmartLightsControlPanel::RespondToBuildableLightSourceListUpdated);
			mSmartLightsControlPanelSubsystem->OnLightSourceStateChanged.AddDynamic(this, &ASmartLightsControlPanel::RespondToLightSourceStateChange);
		}
	}
	}

void ASmartLightsControlPanel::SetAvailableLightsForControlPanel() {
	if (HasAuthority()) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::SetAvailableLightsForControlPanel Auth"));
		Server_SetAvailableLightsForControlPanel();
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::SetAvailableLightsForControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->SetAvailableLightsForControlPanel(this);
	}
}

void ASmartLightsControlPanel::Server_SetAvailableLightsForControlPanel() {
	this->SetBuildableLightConnectionCount(0);
	this->mBuildableLightingConnections = mSmartLightsControlPanelSubsystem->GetControlPanelLightSources(this);
}

void ASmartLightsControlPanel::AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel"));
	if (HasAuthority()) {
		this->Server_AddLightConnectionToControlPanel(BuildableLightingConnection);
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->AddLightConnectionToControlPanel(this, BuildableLightingConnection);
	}
}

void ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel"));
	/*
	* Fix the ceiling light connection issue, im assuming this is also an issue with floodlight stuff 
	*/

	//if (!BuildableLightingConnection.mBuildablePowerConnection) {
	BuildableLightingConnection.mBuildablePowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel %s LightPowerConnection %s"), *(BuildableLightingConnection.mBuildableLightSource->GetName()), (BuildableLightingConnection.mBuildablePowerConnection ? TEXT("Has PowerConnection") : TEXT("No PowerConnection")));
	//}

	ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
	int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel Index: %d"), BuildableConnectionIndex);
	//mCircuitSubsystem->ConnectComponents(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel power conn CircuitGroupId: %d"), BuildableLightingConnection.mBuildablePowerConnection->GetPowerCircuit()->GetCircuitGroupID());
	NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, mSmartPanelDownstreamConnection);
	mSmartPanelDownstreamConnection->AddHiddenConnection(BuildableLightingConnection.mBuildablePowerConnection);
	
	BuildableLightingConnection.mBuildableWire = NewBuildableWire;
	BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(this->IsLightEnabled());
	BuildableLightingConnection.mBuildableLightSource->SetLightControlData(this->GetLightControlData());
	BuildableLightingConnection.isConnected = true;
	BuildableLightingConnection.mShouldShow = true;
	mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
	mSmartLightsControlPanelSubsystem->OnControlPanelToLightConnectionUpdate(this);
}

void ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel"));
	if (HasAuthority()) {
		this->Server_RemoveLightConnectionFromControlPanel(BuildableLightingConnection);
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->RemoveLightConnectionFromControlPanel(this, BuildableLightingConnection);
	}
}

void ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel"));

	//mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, (BuildableLightingConnection.mBuildablePowerConnection 
	//		? BuildableLightingConnection.mBuildablePowerConnection : mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)));
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
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel Connection hasBuildableWire"));
		BuildableLightingConnection.mBuildableWire->Disconnect();
		BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
		BuildableLightingConnection.mBuildableWire->Destroy(true);
	//}
	//else {
	//	UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel Connection hasNoBuildableWire"));
	}

	mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, (mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)));
	mSmartPanelDownstreamConnection->RemoveHiddenConnection(BuildableLightingConnection.mBuildablePowerConnection);

	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel buildableLightSource circuitId: %d"), (mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)->GetCircuitID()));

	int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
	BuildableLightingConnection.isConnected = false;
	BuildableLightingConnection.mShouldShow = true;
	BuildableLightingConnection.mBuildableWire = nullptr;
	mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;

	mSmartLightsControlPanelSubsystem->OnControlPanelToLightConnectionUpdate(this);

	//mDirtyIndex = BuildableConnectionIndex;
	//mIsDirtyList = true;
	//OnRep_ControlPanelBuildableLightingConnections();
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel End %s max conns: %d num conns: %d, num free conns: %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetMaxNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumFreeConnections());
}

TArray< FBuildableLightingConnection> ASmartLightsControlPanel::GetBuildableLightingConnections(ELightSourceType LightSourceType) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections %s"), (HasAuthority() ? TEXT("HAS AUTH") : TEXT("NO AUTH")));
	if (HasAuthority()) {
		if (mSmartLightsControlPanelSubsystem->mBuildableLightSources.Num() != mBuildableLightConnectionCount) {
			this->SetAvailableLightsForControlPanel();
		}
	}
	else {
		USmartWirelessLightingRemoteCallObject* rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		//UE_LOG(LogSWL, Warning, TEXT("ASmartLightsControlPanel::GetBuildableLightingConnections nonAuthority RCO: %s"), (rco ? TEXT("YES") : TEXT("NO")));
		 rco->SetAvailableLightsForControlPanel(this);
		//if (GetWorld()) {
		//	UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel Yes GetWorld"));
		//	AFGPlayerController* playerController = Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController());
		//	if (playerController) {
		//		UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel Yes playerController"));
		//		auto testRco = playerController->GetRemoteCallObjectOfClass(USmartWirelessLightingRemoteCallObject::StaticClass());
		//		if (testRco) {
		//			UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel Yes testRco"));
		//		}
		//		else {
		//			UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel No testRco"));
		//		}
		//	}
		//	else {
		//		UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel No playerController"));
		//	}

		//}
		//else {
		//	UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel No GetWorld"));
		//}
	}
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections mBuildableLightingConnections %d"), mBuildableLightingConnections.Num());
	return mBuildableLightingConnections.FilterByPredicate([&LightSourceType](FBuildableLightingConnection BuildableLightingConnection) {
		return BuildableLightingConnection.mLightSourceType == LightSourceType;
	});

	//return mBuildableLightingConnections;
}

void ASmartLightsControlPanel::UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelStatus"));
	if (HasAuthority()) {
		Server_UpdateControlPanelStatus(IsEnabled);
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelStatus No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateControlPanelStatus(this, IsEnabled);
	}
}

void ASmartLightsControlPanel::Server_UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus"));
	Super::SetLightEnabled(IsEnabled);
	
	TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus LightSourcesNum: %d"), BuildableLightSources.Num());

	for (AFGBuildable* Buildable : BuildableLightSources) {
		AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
		if (lightSource) {
			lightSource->SetLightEnabled(IsEnabled);
		}
	}

}

void ASmartLightsControlPanel::UpdateLightControlData(FLightSourceControlData Data) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData"));
	if (HasAuthority()) {
		Server_UpdateLightControlData(Data);
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateLightControlData(this, Data);
	}
}

void ASmartLightsControlPanel::Server_UpdateLightControlData(FLightSourceControlData Data) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus Data: %f"), Data.Intensity);

	Super::SetLightControlData(Data);

	TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	

	for (AFGBuildable* Buildable : BuildableLightSources) {
		AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
		if (lightSource) {
			lightSource->SetLightControlData(Data);
		}
	}

}

void ASmartLightsControlPanel::UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	if (HasAuthority()) {
		Server_UpdateLightColorSlot(slotIdx, NewColor);
	}
	else {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateLightColorSlot(this, slotIdx, NewColor);
	}
}

void ASmartLightsControlPanel::Server_UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	mSmartLightsControlPanelSubsystem->UpdateLightColorSlot(slotIdx, NewColor);
}

void ASmartLightsControlPanel::RespondToBuildableLightSourceListUpdated() {
	if (HasAuthority()) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RespondToBuildableLightSourceListUpdated"));
		SetBuildableLightConnectionCount(-1);
		this->SetAvailableLightsForControlPanel();
		//SetBuildableLightConnectionCount(mSmartLightsControlPanelSubsystem->mBuildableLightSources.Num());
	}
}

void ASmartLightsControlPanel::RespondToLightSourceStateChange(class ASmartLightsControlPanel* controlPanel) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RespondToLightSourceStateChange this name %s"), *(this->GetName()));
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RespondToLightSourceStateChange controlPanel name %s"), *(controlPanel->GetName()));
	
	if (!this->GetName().Equals(controlPanel->GetName())) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RespondToLightSourceStateChange this != controlPanel"));
		this->SetAvailableLightsForControlPanel();
	}
}

void ASmartLightsControlPanel::RespondToLightColorSlotUpdate() {
	
}

void ASmartLightsControlPanel::SetBuildableLightConnectionCount(uint8 BuildableLightConnectionCount) {
	mBuildableLightConnectionCount = BuildableLightConnectionCount;
}


///*
// * Keeping this here for my sake..gonna try to implmement this at some point..at this point, just dont feel like working on this bug.
// * Refreshing the list isn't that intensive, at least not yet..
// */
////void ASmartLightsControlPanel::BindTo_OnLightConnectionChanged(class UFGCircuitConnectionComponent* CircuitConnection)
////{
////	UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::BindTo_OnLightConnectionChanged %s %d"), HasAuthority() ? TEXT("Auth") : TEXT("Remote"), CircuitConnection->GetCircuitID());
////	if (HasAuthority()) {
////		CircuitConnection->OnConnectionChanged.RemoveAll(this);
////		RefreshControlPanelBucket();
////	}
////}

void ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections %s"), HasAuthority() ? TEXT("auth") : TEXT("remote"));
	//mIsDirtyList = true;
}
