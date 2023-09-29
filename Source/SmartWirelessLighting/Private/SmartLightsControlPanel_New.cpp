#include "SmartLightsControlPanel_New.h"

#include "Build_SmartWirelessWireBase.h"
#include "SmartLightsControlPanelSubsystem.h"
#include "SmartWirelessLightingRemoteCallObject_New.h"

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

ASmartLightsControlPanel_New::ASmartLightsControlPanel_New() : Super() 
{
	this->bReplicates = true;
}

void ASmartLightsControlPanel_New::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmartLightsControlPanel_New, mBuildableLightingConnections);
}

void ASmartLightsControlPanel_New::OnDismantleEffectFinished()
{
	Super::OnDismantleEffectFinished();
	//if (HasAuthority()) {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::OnDismantleEffectFinished"));
	//	for (FBuildableLightingConnection_New LightingConnection : this->mBuildableLightingConnections)
	//	{
	//		if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
	//		{
	//			LightingConnection.mBuildableWire->Destroy(true);
	//			LightingConnection.mBuildableWire->TurnOffAndDestroy();
	//		}
	//	}
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::OnDismantleEffectFinished No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->OnDismantleEffectFinishedDestroyConnections(this);
	//}
}

void ASmartLightsControlPanel_New::Server_OnDismantleEffectFinished()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_OnDismantleEffectFinished"));
	//for (FBuildableLightingConnection_New LightingConnection : this->mBuildableLightingConnections)
	//{
	//	if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
	//	{
	//		LightingConnection.mBuildableWire->Destroy(true);
	//		LightingConnection.mBuildableWire->TurnOffAndDestroy();
	//	}
	//}

}

void ASmartLightsControlPanel_New::BeginPlay()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::BeginPlay"));
	Super::BeginPlay();
	//AFGCircuitSubsystem* CircuitSubsystem = AFGCircuitSubsystem::Get(GetWorld());
	//mSmartPanelDownstreamConnection = Super::mDownstreamConnection;

	////if (!HasAuthority()) return;

	//if (CircuitSubsystem) {
	//	mCircuitSubsystem = CircuitSubsystem;
	//}

	//ASmartLightsControlPanelSubsystem* SmartLightsControlPanelSubsystem = ASmartLightsControlPanelSubsystem::getSubsystem(GetWorld());

	//if (SmartLightsControlPanelSubsystem) {
	//	mSmartLightsControlPanelSubsystem = SmartLightsControlPanelSubsystem;
	//	if (HasAuthority()) {
	//		mSmartLightsControlPanelSubsystem->GetAllLightSources();
	//		this->SetAvailableLightsForControlPanel();
	//		mSmartLightsControlPanelSubsystem->OnBuildableLightSourceStateChanged.AddDynamic(this, &ASmartLightsControlPanel_New::RespondToBuildableLightSourceListUpdated);
	//	}

	//	/*mLightSourceControlData = this->GetLightControlData();*/

	//}

}

void ASmartLightsControlPanel_New::SetAvailableLightsForControlPanel() {
	//if (HasAuthority()) {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::SetAvailableLightsForControlPanel Auth"));
	//	Server_SetAvailableLightsForControlPanel();
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::SetAvailableLightsForControlPanel No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->SetAvailableLightsForControlPanel(this);
	//}
}

void ASmartLightsControlPanel_New::Server_SetAvailableLightsForControlPanel() {
	//this->mBuildableLightingConnections = mSmartLightsControlPanelSubsystem->GetControlPanelLightSources(this);
}

void ASmartLightsControlPanel_New::AddLightConnectionToControlPanel(FBuildableLightingConnection_New BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::AddLightConnectionToControlPanel"));
	//if (HasAuthority()) {
	//	this->Server_AddLightConnectionToControlPanel(BuildableLightingConnection);
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::SetAvailableLightsForControlPanel No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->AddLightConnectionToControlPanel(this, BuildableLightingConnection);
	//}
}

void ASmartLightsControlPanel_New::Server_AddLightConnectionToControlPanel(FBuildableLightingConnection_New BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_AddLightConnectionToControlPanel"));
	//ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
	//int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	////mCircuitSubsystem->ConnectComponents(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
	////UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel power conn CircuitGroupId: %d"), BuildableLightingConnection.mBuildablePowerConnection->GetPowerCircuit()->GetCircuitGroupID());
	//NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, mSmartPanelDownstreamConnection);

	//BuildableLightingConnection.mBuildableWire = NewBuildableWire;
	//BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(this->IsLightEnabled());
	//BuildableLightingConnection.mBuildableLightSource->SetLightControlData(this->GetLightControlData());
	//BuildableLightingConnection.isConnected = true;
	//BuildableLightingConnection.mShouldShow = true;
	//mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
}

void ASmartLightsControlPanel_New::RemoveLightConnectionFromControlPanel(FBuildableLightingConnection_New BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::RemoveLightConnectionFromControlPanel"));
	//if (HasAuthority()) {
	//	this->Server_RemoveLightConnectionFromControlPanel(BuildableLightingConnection);
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::RemoveLightConnectionFromControlPanel No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->RemoveLightConnectionFromControlPanel(this, BuildableLightingConnection);
	//}
}

void ASmartLightsControlPanel_New::Server_RemoveLightConnectionFromControlPanel(FBuildableLightingConnection_New BuildableLightingConnection) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_RemoveLightConnectionFromControlPanel"));
	//mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, BuildableLightingConnection.mBuildablePowerConnection);
	//if (!BuildableLightingConnection.mBuildableWire)
	//{
	//	AFGBuildableWire* BuildableWire = mSmartLightsControlPanelSubsystem->GetControlPanelToLightWire(BuildableLightingConnection.mBuildablePowerConnection, mSmartPanelDownstreamConnection);
	//	if (BuildableWire) 
	//	{
	//		BuildableLightingConnection.mBuildableWire = BuildableWire;
	//	}
	//}
	//
	//if (BuildableLightingConnection.mBuildableWire) {
	//	BuildableLightingConnection.mBuildableWire->Disconnect();
	//	BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
	//	BuildableLightingConnection.mBuildableWire->Destroy(true);
	//}

	//int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
	//BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
	//BuildableLightingConnection.isConnected = false;
	//BuildableLightingConnection.mShouldShow = true;
	//BuildableLightingConnection.mBuildableWire = nullptr;
	//mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;

	//mDirtyIndex = BuildableConnectionIndex;
	//mIsDirtyList = true;
	//OnRep_ControlPanelBuildableLightingConnections();
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel End %s max conns: %d num conns: %d, num free conns: %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetMaxNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumFreeConnections());
}

TArray< FBuildableLightingConnection_New> ASmartLightsControlPanel_New::GetBuildableLightingConnections(ELightSourceType_New LightSourceType) {

	//if (HasAuthority()) {
	//	if (mSmartLightsControlPanelSubsystem->mBuildableLightSources.Num() != mBuildableLightConnectionCount) {
	//		this->SetAvailableLightsForControlPanel();
	//	}
	//}
	//else {
	//	this->SetAvailableLightsForControlPanel();
	//}
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::GetBuildableLightingConnections mBuildableLightingConnections %d"), mBuildableLightingConnections.Num());
	//return mBuildableLightingConnections.FilterByPredicate([&LightSourceType](FBuildableLightingConnection_New BuildableLightingConnection) {
	//	return BuildableLightingConnection.mLightSourceType == LightSourceType;
	//});

	return mBuildableLightingConnections;
}

void ASmartLightsControlPanel_New::UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::UpdateControlPanelStatus"));
	//if (HasAuthority()) {
	//	Server_UpdateControlPanelStatus(IsEnabled);
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::UpdateControlPanelStatus No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->UpdateControlPanelStatus(this, IsEnabled);
	//}
}

void ASmartLightsControlPanel_New::Server_UpdateControlPanelStatus(bool IsEnabled) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_UpdateControlPanelStatus"));
	Super::SetLightEnabled(IsEnabled);
	//
	//TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	//TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	////UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_UpdateControlPanelStatus LightSourcesNum: %d"), BuildableLightSources.Num());

	//for (AFGBuildable* Buildable : BuildableLightSources) {
	//	AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
	//	if (lightSource) {
	//		lightSource->SetLightEnabled(IsEnabled);
	//	}
	//}

}

void ASmartLightsControlPanel_New::UpdateLightControlData(FLightSourceControlData Data) {
	UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanel_New::UpdateLightControlData"));
	//if (HasAuthority()) {
	//	Server_UpdateLightControlData(Data);
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::UpdateLightControlData No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->UpdateLightControlData(this, Data);
	//}
}

void ASmartLightsControlPanel_New::Server_UpdateLightControlData(FLightSourceControlData Data) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::Server_UpdateControlPanelStatus Data: %f"), Data.Intensity);

	Super::SetLightControlData(Data);

	//TSubclassOf< AFGBuildable > output = AFGBuildableLightSource::StaticClass();
	//TArray<AFGBuildable*> BuildableLightSources = GetControlledBuildables(output);
	//

	//for (AFGBuildable* Buildable : BuildableLightSources) {
	//	AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(Buildable);
	//	if (lightSource) {
	//		lightSource->SetLightControlData(Data);
	//	}
	//}

}

void ASmartLightsControlPanel_New::UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	//if (HasAuthority()) {
	//	Server_UpdateLightColorSlot(slotIdx, NewColor);
	//}
	//else {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::UpdateLightControlData No Auth"));
	//	auto rco = USmartWirelessLightingRemoteCallObject_New::getRCO(GetWorld());
	//	rco->UpdateLightColorSlot(this, slotIdx, NewColor);
	//}
}

void ASmartLightsControlPanel_New::Server_UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	//mSmartLightsControlPanelSubsystem->UpdateLightColorSlot(slotIdx, NewColor);
}

void ASmartLightsControlPanel_New::RespondToBuildableLightSourceListUpdated() {
	//if (HasAuthority()) {
	//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel_New::RespondToBuildableLightSourceListUpdated"));
	//	SetBuildableLightConnectionCount(mSmartLightsControlPanelSubsystem->mBuildableLightSources.Num());
	//}
}

void ASmartLightsControlPanel_New::RespondToLightColorSlotUpdats() {

}

void ASmartLightsControlPanel_New::SetBuildableLightConnectionCount(uint8 BuildableLightConnectionCount) {
	//mBuildableLightConnectionCount = BuildableLightConnectionCount;
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

void ASmartLightsControlPanel_New::OnRep_ControlPanelBuildableLightingConnections()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections %s"), HasAuthority() ? TEXT("auth") : TEXT("remote"));
	//mIsDirtyList = true;
}
