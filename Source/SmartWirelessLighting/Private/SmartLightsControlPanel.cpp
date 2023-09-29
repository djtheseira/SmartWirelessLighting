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
	DOREPLIFETIME(ASmartLightsControlPanel, mBuildableLightConnectionCount);
}

void ASmartLightsControlPanel::OnDismantleEffectFinished()
{
	Super::OnDismantleEffectFinished();
	if (HasAuthority()) {
		this->Server_OnDismantleEffectFinished();
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished"));
		//for (FBuildableLightingConnection LightingConnection : this->mBuildableLightingConnections)
		//{
		//	if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
		//	{
		//		LightingConnection.mBuildableWire->Destroy(true);
		//		LightingConnection.mBuildableWire->TurnOffAndDestroy();
		//	}
		//}
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->OnDismantleEffectFinishedDestroyConnections(this);
	}
}

void ASmartLightsControlPanel::Server_OnDismantleEffectFinished()
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_OnDismantleEffectFinished"));
	for (FBuildableLightingConnection LightingConnection : this->mBuildableLightingConnections)
	{
		if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
		{
			LightingConnection.mBuildableWire->Destroy(true);
			LightingConnection.mBuildableWire->TurnOffAndDestroy();
		}
	}
	mSmartLightsControlPanelSubsystem->OnSmartWirelessLightControlPanelDestroyed();
}

void ASmartLightsControlPanel::BeginPlay()
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::BeginPlay"));
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
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::BeginPlay mSmartLightsControlPanelSubsystem %s"), mSmartLightsControlPanelSubsystem ? TEXT("YEA") : TEXT("NAH"));
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
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::SetAvailableLightsForControlPanel Auth"));
		Server_SetAvailableLightsForControlPanel();
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::SetAvailableLightsForControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->SetAvailableLightsForControlPanel(this);
	}
}

void ASmartLightsControlPanel::Server_SetAvailableLightsForControlPanel() {
	//this->SetBuildableLightConnectionCount(0);
	this->mBuildableLightingConnections = mSmartLightsControlPanelSubsystem->GetControlPanelLightSources(this);
	this->SetBuildableLightConnectionCount(mBuildableLightingConnections.Num());
	//isListStale = true;
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::SetAvailableLightsForControlPanel connectionsSize: %d"), mBuildableLightingConnections.Num());
}

void ASmartLightsControlPanel::UpdateAvailableLightListIsDirty() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateAvailableLightListIsDirty"));
	if (HasAuthority()) {
		this->Server_UpdateAvailableLightListIsDirty();
	}
	else {
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateAvailableLightIsDirty(this);
	}
}

void ASmartLightsControlPanel::Server_UpdateAvailableLightListIsDirty() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateAvailableLightListIsDirty"));
	mSmartLightsControlPanelSubsystem->SetLightListIsDirty(true);
	this->SetBuildableLightConnectionCount(0);
}

void ASmartLightsControlPanel::UpdateAvailableLightList() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateAvailableLightList"));
	if (HasAuthority()) {
		this->Server_UpdateAvailableLightList();
	}
	else {
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateAvailableLights(this);
	}
}

void ASmartLightsControlPanel::Server_UpdateAvailableLightList() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateAvailableLightList"));
	mSmartLightsControlPanelSubsystem->GetAllLightSources();
	this->mBuildableLightingConnections = mSmartLightsControlPanelSubsystem->GetControlPanelLightSources(this);
	this->SetBuildableLightConnectionCount(mBuildableLightingConnections.Num());
	
}

void ASmartLightsControlPanel::AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel"));
	if (HasAuthority()) {
		this->Server_AddLightConnectionToControlPanel(BuildableLightingConnection);
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::AddLightConnectionToControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->AddLightConnectionToControlPanel(this, BuildableLightingConnection);
	}
}

void ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel"));
	/*
	* Fix the ceiling light connection issue, im assuming this is also an issue with floodlight stuff 
	*/

	//if (!BuildableLightingConnection.mBuildablePowerConnection) {
	BuildableLightingConnection.mBuildablePowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel %s LightPowerConnection %s"), *(BuildableLightingConnection.mBuildableLightSource->GetName()), (BuildableLightingConnection.mBuildablePowerConnection ? TEXT("Has PowerConnection") : TEXT("No PowerConnection")));
	//}

	ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
	int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_AddLightConnectionToControlPanel Index: %d"), BuildableConnectionIndex);
	//mCircuitSubsystem->ConnectComponents(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel power conn CircuitGroupId: %d"), BuildableLightingConnection.mBuildablePowerConnection->GetPowerCircuit()->GetCircuitGroupID());
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
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel"));
	if (HasAuthority()) {
		this->Server_RemoveLightConnectionFromControlPanel(BuildableLightingConnection);
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightConnectionFromControlPanel No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->RemoveLightConnectionFromControlPanel(this, BuildableLightingConnection);
	}
}

void ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel(FBuildableLightingConnection BuildableLightingConnection) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel"));

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
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel Connection hasBuildableWire"));
		BuildableLightingConnection.mBuildableWire->Disconnect();
		BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
		BuildableLightingConnection.mBuildableWire->Destroy(true);
	//}
	//else {
	//	UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel Connection hasNoBuildableWire"));
	}

	mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, (mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)));
	mSmartPanelDownstreamConnection->RemoveHiddenConnection(BuildableLightingConnection.mBuildablePowerConnection);

	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_RemoveLightConnectionFromControlPanel buildableLightSource circuitId: %d"), (mSmartLightsControlPanelSubsystem->GetLightSourcePowerConnectionComponent(BuildableLightingConnection.mBuildableLightSource)->GetCircuitID()));

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
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel End %s max conns: %d num conns: %d, num free conns: %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetMaxNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumFreeConnections());
}

TArray< FBuildableLightingConnection> ASmartLightsControlPanel::GetBuildableLightingConnections(ELightSourceType LightSourceType) {
	this->UpdateAvailableLightList();

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

	//return mBuildableLightingConnections;
}

void ASmartLightsControlPanel::UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelStatus"));
	if (HasAuthority()) {
		Server_UpdateControlPanelStatus(IsEnabled);
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelStatus No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateControlPanelStatus(this, IsEnabled);
	}
}

void ASmartLightsControlPanel::Server_UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus"));
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
	if (HasAuthority()) {
		Server_UpdateLightControlData(Data);
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Verbose, TEXT(".ASmartLightsControlPanel::UpdateLightControlData No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateLightControlData(this, Data);
	}
}

void ASmartLightsControlPanel::Server_UpdateLightControlData(FLightSourceControlData Data) {
	//UE_LOG(LogSmartWirelessLighting, Verbose, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus Data: %f"), Data.Intensity);
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus Data: %f"), Data.ColorSlotIndex);
	Super::SetLightControlData(Data);

	TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	

	for (AFGBuildable* Buildable : BuildableLightSources) {
		AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
		if (lightSource) {
			lightSource->SetLightControlData(Data);
			//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Server_UpdateControlPanelStatus LightSource Intensity: %f"), lightSource->GetLightControlData().Intensity);
		}
	}

}

void ASmartLightsControlPanel::UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	if (HasAuthority()) {
		Server_UpdateLightColorSlot(slotIdx, NewColor);
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData No Auth"));
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->UpdateLightColorSlot(this, slotIdx, NewColor);
	}
}

void ASmartLightsControlPanel::Server_UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	mSmartLightsControlPanelSubsystem->UpdateLightColorSlot(slotIdx, NewColor);
}

void ASmartLightsControlPanel::RespondToBuildableLightSourceListUpdated() {
	if (HasAuthority()) {
		this->Multicast_RespondToBuildableLightSourceListUpdated();
	}
}

void ASmartLightsControlPanel::Multicast_RespondToBuildableLightSourceListUpdated_Implementation() {
	if (HasAuthority()) {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Multicast_RespondToBuildableLightSourceListUpdated_Implementation auth"));
		this->SetAvailableLightsForControlPanel();
	}
	else {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Multicast_RespondToBuildableLightSourceListUpdated_Implementation noauth"));
		//SetBuildableLightConnectionCount(-1);
		auto rco = USmartWirelessLightingRemoteCallObject::getRCO(GetWorld());
		rco->SetAvailableLightsForControlPanel(this);
	}
}

void ASmartLightsControlPanel::RespondToLightSourceStateChange(class ASmartLightsControlPanel* controlPanel) {
	
	if (HasAuthority() && !this->GetName().Equals(controlPanel->GetName())) {
		Multicast_RespondToLightSourceStateChange();
	}
}

void ASmartLightsControlPanel::Multicast_RespondToLightSourceStateChange_Implementation() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel::Multicast_RespondToLightSourceStateChange_Implementation"));
	this->SetBuildableLightConnectionCount(-1);
	this->SetAvailableLightsForControlPanel();
	
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
