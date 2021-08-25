#include "SmartLightsControlPanel.h"
#include "Build_SmartWirelessWireBase.h"
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
#include "Net/UnrealNetwork.h"

ASmartLightsControlPanel::ASmartLightsControlPanel() : Super() 
{
	this->SetReplicates(true);
	this->bReplicates = true;
	this->NetDormancy = DORM_Initial;
	this->NetCullDistanceSquared = 5624999936;
	this->mHighlightParticleClassName = FSoftClassPath("/Game/FactoryGame/Buildable/-Shared/Particle/NewBuildingPing.NewBuildingPing_C");
	this->mDismantleEffectClassName = FSoftClassPath("/Game/FactoryGame/Buildable/Factory/-Shared/BP_MaterialEffect_Dismantle.BP_MaterialEffect_Dismantle_C");
	this->mBuildEffectClassName = FSoftClassPath("/Game/FactoryGame/Buildable/Factory/-Shared/BP_MaterialEffect_Build.BP_MaterialEffect_Build_C");
}

void ASmartLightsControlPanel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ASmartLightsControlPanel, mControlPanelSmartLightingBucket);
	DOREPLIFETIME(ASmartLightsControlPanel, mIsFirstUpdate);
	DOREPLIFETIME(ASmartLightsControlPanel, mIsDirtyList);
	DOREPLIFETIME(ASmartLightsControlPanel, mBuildableLightingConnections);
	DOREPLIFETIME(ASmartLightsControlPanel, mControlPanel);
}

void ASmartLightsControlPanel::OnDismantleEffectFinished()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished"));
	for (FBuildableLightingConnection LightingConnection : this->mBuildableLightingConnections)
	{
		if (LightingConnection.mBuildableWire && LightingConnection.isConnected)
		{
			LightingConnection.mBuildableWire->Destroy(true);
			LightingConnection.mBuildableWire->TurnOffAndDestroy();
		}
	}
	Super::OnDismantleEffectFinished();
}

void ASmartLightsControlPanel::BeginPlay()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::BeginPlay"));
	Super::BeginPlay();
	AFGCircuitSubsystem* CircuitSubsystem = AFGCircuitSubsystem::Get(GetWorld());
	if (CircuitSubsystem) {
		mCircuitSubsystem = CircuitSubsystem;
	}
}

void ASmartLightsControlPanel::SetupVariables(class UFGCircuitConnectionComponent* DownstreamConnection, class ASmartLightsControlPanel* ControlPanel, TArray<FBuildableLightingConnection>& BuildableLightingConnections)
{
	if (HasAuthority()) 
	{
		mDownstreamConnection = DownstreamConnection;
		mControlPanel = ControlPanel;
		mBuildableLightingConnections = BuildableLightingConnections;
		this->RefreshControlPanelBucket();
	}
}

void ASmartLightsControlPanel::RefreshControlPanelBucket_Implementation()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RefreshControlPanelBucket Start ControlPanel: %s %d"), *mControlPanel->GetName(), mBuildableLightingConnections.Num());
	int32 DownstreamConnectionCircuitId = mDownstreamConnection->GetCircuitID();
	UFGPowerConnectionComponent* DownstreamPowerConnection = Cast<UFGPowerConnectionComponent>(mDownstreamConnection);
	TArray<FBuildableLightingConnection> NewBuildableLightingConnections = mBuildableLightingConnections;
	for (FBuildableLightingConnection LightingConnection : NewBuildableLightingConnections)
	{
		if (!LightingConnection.mBuildablePowerConnection)
		{
			UFGPowerConnectionComponent* LightConnection = Cast<UFGPowerConnectionComponent>(LightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
			LightingConnection.mBuildablePowerConnection = LightConnection;
		}

		if (!LightingConnection.mBuildableLightSource->OnDestroyed.Contains(this, "OnLightingConnectionDestoryed")) {
			LightingConnection.mBuildableLightSource->OnDestroyed.AddDynamic(this, &ASmartLightsControlPanel::OnLightingConnectionDestoryed);
		}

		TArray< UFGCircuitConnectionComponent* >& LightConnections = *(new TArray< UFGCircuitConnectionComponent*>);
		LightingConnection.mBuildablePowerConnection->GetConnections(LightConnections);

		int32 ConnectionIndex = NewBuildableLightingConnections.Find(LightingConnection);
		if (LightingConnection.mBuildablePowerConnection->GetCircuitID() > -1 && !LightConnections.Contains(this->mDownstreamConnection))
		{
			LightingConnection.mShouldShow = false;
			LightingConnection.isConnected = false;
		}
		else
		{
			LightingConnection.isConnected = LightConnections.Contains(mDownstreamConnection);
			if (LightingConnection.isConnected) {
				LightingConnection.mBuildableWire = this->GetControlPanelToLightWire(LightingConnection.mBuildablePowerConnection);
			}
			LightingConnection.mShouldShow = true;
		}
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RefreshControlPanelBucket %s: %s conn; %s show"), *LightingConnection.mBuildableLightSource->GetName(), LightingConnection.isConnected ? TEXT("is") : TEXT("is not"), LightingConnection.mShouldShow ? TEXT("should") : TEXT("should not"));
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RefreshControlPanelBucket %s: circuitid: %d"), *LightingConnection.mBuildableLightSource->GetName(), LightingConnection.mBuildablePowerConnection->GetCircuitID());
		LightingConnection.mLightSourceType = this->GetBuildableLightSourceType(LightingConnection.mBuildableLightSource->GetName());
		LightingConnection.mDistanceToControlPanel = floorf(mControlPanel->GetHorizontalDistanceTo(LightingConnection.mBuildableLightSource)) / 100;
		NewBuildableLightingConnections[ConnectionIndex] = LightingConnection;
		//mBuildableLightingConnections.Insert(LightingConnection, ConnectionIndex);
	}

	if (NewBuildableLightingConnections.Num() > 0)
	{
		NewBuildableLightingConnections.Sort([](const FBuildableLightingConnection& A, const FBuildableLightingConnection& B)
		{
			if (A.isConnected) {
				return true;
			}
			else if (B.isConnected) {
				return false;
			}
			return islessequal(A.mDistanceToControlPanel, B.mDistanceToControlPanel);
		});
	}
	mBuildableLightingConnections = NewBuildableLightingConnections;
	mIsDirtyList = false;
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RefreshControlPanelBucket Finished ControlPanel: %s %d"), *mControlPanel->GetName(), mBuildableLightingConnections.Num());
}

void ASmartLightsControlPanel::AddBuildableLightSource_Implementation(class AFGBuildableLightSource* LightSource)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddBuildableLightSource"));
	if (LightSource)
	{
		FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
		LightingConnection.mBuildableLightSource = LightSource;
		LightingConnection.mLightSourceType = this->GetBuildableLightSourceType(LightingConnection.mBuildableLightSource->GetName());
		LightingConnection.mDistanceToControlPanel = floorf(mControlPanel->GetHorizontalDistanceTo(LightingConnection.mBuildableLightSource)) / 100;
		UFGPowerConnectionComponent* LightConnection = Cast<UFGPowerConnectionComponent>(LightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
		if (LightConnection) {
			LightingConnection.mBuildablePowerConnection = LightConnection;
			LightingConnection.mShouldShow = LightConnection->GetNumConnections() == 0;
		}
		int32 DirtyIndex = mBuildableLightingConnections.Add(LightingConnection);
		mDirtyIndex = DirtyIndex;
	}
}

void ASmartLightsControlPanel::RemoveBuildableLightSource_Implementation(class AFGBuildableLightSource* LightSource)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveBuildableLightSource"));
	if (mBuildableLightingConnections.Num() > 0) 
	{
		int32 KeyIndex = mBuildableLightingConnections.IndexOfByKey(LightSource);
		if (KeyIndex > -1) {
			if (mBuildableLightingConnections[KeyIndex].mBuildableWire)
			{
				mBuildableLightingConnections[KeyIndex].mBuildableWire->TurnOffAndDestroy();
				mBuildableLightingConnections[KeyIndex].mBuildableWire->Destroy(true);
			}

			mBuildableLightingConnections.RemoveAt(KeyIndex, 1, true);
		}
	}
}

void ASmartLightsControlPanel::AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel Start %s %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetCircuitID());
	if (HasAuthority() && mCircuitSubsystem && BuildableLightingConnection.mBuildablePowerConnection->GetNumFreeConnections() > 0)
	{
		ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
		int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
		//mCircuitSubsystem->ConnectComponents(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel power conn CircuitGroupId: %d"), BuildableLightingConnection.mBuildablePowerConnection->GetPowerCircuit()->GetCircuitGroupID());
		NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
		BuildableLightingConnection.mBuildableWire = NewBuildableWire;
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(mControlPanel->IsLightEnabled());
		BuildableLightingConnection.mBuildableLightSource->SetLightControlData(mControlPanel->GetLightControlData());
		BuildableLightingConnection.isConnected = true;
		BuildableLightingConnection.mShouldShow = true;
		mDirtyIndex = BuildableConnectionIndex;
		mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
		mIsDirtyList = true;
		OnRep_ControlPanelBuildableLightingConnections();
	}

	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel End %s %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetCircuitID());
}

void ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel Start %s %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetCircuitID());
	if (HasAuthority())
	{
		int32 BuildableConnectionIndex = mBuildableLightingConnections.Find(BuildableLightingConnection);
		mCircuitSubsystem->DisconnectComponents(mDownstreamConnection, BuildableLightingConnection.mBuildablePowerConnection);
		if (!BuildableLightingConnection.mBuildableWire)
		{
			AFGBuildableWire* BuildableWire = GetControlPanelToLightWire(BuildableLightingConnection.mBuildablePowerConnection);
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
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
		BuildableLightingConnection.isConnected = false;
		BuildableLightingConnection.mShouldShow = true;
		BuildableLightingConnection.mBuildableWire = nullptr;
		mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
		mDirtyIndex = BuildableConnectionIndex;
		mIsDirtyList = true;
		OnRep_ControlPanelBuildableLightingConnections();
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel End %s max conns: %d num conns: %d, num free conns: %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetMaxNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumConnections(), BuildableLightingConnection.mBuildablePowerConnection->GetNumFreeConnections());
	}
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel End %s %d"), *BuildableLightingConnection.mBuildableLightSource->GetName(), BuildableLightingConnection.mBuildablePowerConnection->GetCircuitID());
}

void ASmartLightsControlPanel::UpdateLightingConnection_Implementation(FBuildableLightingConnection LightingConnection)
{
	int32 ConnectionIndex = mBuildableLightingConnections.Find(LightingConnection);
	LightingConnection.mShouldShow = !mBuildableLightingConnections[ConnectionIndex].mShouldShow;
	LightingConnection.isConnected = !mBuildableLightingConnections[ConnectionIndex].isConnected;
	if (!LightingConnection.isConnected)
	{
		LightingConnection.mBuildableWire = nullptr;
	}
	mBuildableLightingConnections[ConnectionIndex] = LightingConnection;
}

void ASmartLightsControlPanel::UpdateLightControlData_Implementation(FLightSourceControlData data)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData"));
	mControlPanel->SetLightControlData(data);
	for (FBuildableLightingConnection BuildableLightingConnection : mBuildableLightingConnections) {
		if (BuildableLightingConnection.isConnected) {
			BuildableLightingConnection.mBuildableLightSource->SetLightControlData(data);
		}
	}
}

void ASmartLightsControlPanel::UpdateLightStatus_Implementation(bool LightStatus)
{
	mControlPanel->SetLightEnabled(LightStatus);
	for (FBuildableLightingConnection BuildableLightingConnection : mBuildableLightingConnections) {
		if (BuildableLightingConnection.isConnected) {
			BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(LightStatus);
		}
	}
}

void ASmartLightsControlPanel::SetIsDirtyList_Implementation()
{
	if (HasAuthority()) {
		mIsDirtyList = true;
	}
}

//FSmartLightingBucket ASmartLightsControlPanel::GetControlPanelSmartLightingBucket()
//{
//	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetControlPanelSmartLightingBucket Num %d"), mControlPanelSmartLightingBucket.mBuildableLightingConnections.Num());
//	return mControlPanelSmartLightingBucket;
//}

int32 ASmartLightsControlPanel::GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel GetLightingConnectionIndex"));
	return mBuildableLightingConnections.Find(BuildableLightingConnection);
}

TArray<FBuildableLightingConnection>& ASmartLightsControlPanel::GetBuildableLightingConnections()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetBuildableLightingConnections"));
	if (mIsDirtyList && HasAuthority()) {
		RefreshControlPanelBucket();
	}
	return mBuildableLightingConnections;
}

FBuildableLightingConnection ASmartLightsControlPanel::GetDirtyLightingConnection()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetDirtyLightingConnection"));
	return mBuildableLightingConnections[mDirtyIndex];
}

bool ASmartLightsControlPanel::GetIsDirtyList()
{
	return mIsDirtyList;
}

//void ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated()
//{
//	UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated %s"), *mControlPanelSmartLightingBucket.mControlPanel->GetName());
//}

void ASmartLightsControlPanel::OnRep_DirtyIndex()
{

}

void ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelBuildableLightingConnections %s"), HasAuthority() ? TEXT("auth") : TEXT("remote"));
	mIsDirtyList = true;
}

void ASmartLightsControlPanel::OnRep_ControlPanel()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanel %s"), *mControlPanel->GetName());
}

void ASmartLightsControlPanel::OnRep_IsDirtyList()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_IsDirtyList %s"), *mControlPanel->GetName());
}