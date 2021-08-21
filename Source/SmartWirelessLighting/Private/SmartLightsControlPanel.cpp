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
	this->NetDormancy = DORM_Initial;
	this->NetCullDistanceSquared = 5624999936;
	this->mHighlightParticleClassName = FSoftClassPath("/Game/FactoryGame/Buildable/-Shared/Particle/NewBuildingPing.NewBuildingPing_C");
	this->mDismantleEffectClassName = FSoftClassPath("/Game/FactoryGame/Buildable/Factory/-Shared/BP_MaterialEffect_Dismantle.BP_MaterialEffect_Dismantle_C");
	this->mBuildEffectClassName = FSoftClassPath("/Game/FactoryGame/Buildable/Factory/-Shared/BP_MaterialEffect_Build.BP_MaterialEffect_Build_C");
	SetReplicates(true);
	bReplicates = true;
}

void ASmartLightsControlPanel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmartLightsControlPanel, mControlPanelSmartLightingBucket);
	DOREPLIFETIME(ASmartLightsControlPanel, mIsFirstUpdate);
}

void ASmartLightsControlPanel::OnDismantleEffectFinished()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnDismantleEffectFinished"));
	for (FBuildableLightingConnection LightingConnection : mControlPanelSmartLightingBucket.mBuildableLightingConnections)
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

void ASmartLightsControlPanel::UpdateControlPanelSmartLightingBucket_Implementation(FSmartLightingBucket UpdatedSmartLightingBucket)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateControlPanelSmartLightingBucket"));
	int32 DownstreamConnectionCircuitId = Super::mDownstreamConnection->GetCircuitID();
	UFGPowerConnectionComponent* DownstreamPowerConnection = Cast<UFGPowerConnectionComponent>(Super::mDownstreamConnection);
	for (FBuildableLightingConnection LightingConnection : UpdatedSmartLightingBucket.mBuildableLightingConnections)
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
		
		int32 ConnectionIndex = UpdatedSmartLightingBucket.mBuildableLightingConnections.Find(LightingConnection);
		if (LightingConnection.mBuildablePowerConnection->GetCircuitID() > -1 &&  !LightConnections.Contains(Super::mDownstreamConnection))
		{
			LightingConnection.mShouldShow = false;
			LightingConnection.isConnected = false;
		} 
		else 
		{
			LightingConnection.isConnected = LightConnections.Contains(Super::mDownstreamConnection);
			if (LightingConnection.isConnected) {
				LightingConnection.mBuildableWire = this->GetControlPanelToLightWire(LightingConnection.mBuildablePowerConnection);
			}
			LightingConnection.mShouldShow = true;
		}

		if (LightingConnection.mBuildableLightSource->GetName().Contains("Build_StreetLight")) 
		{
			LightingConnection.mLightSourceType = ELightSourceType::LS_StreetLight;
		}
		else if (LightingConnection.mBuildableLightSource->GetName().Contains("Build_CeilingLight")) 
		{
			LightingConnection.mLightSourceType = ELightSourceType::LS_CeilingLight;
		}
		else if (LightingConnection.mBuildableLightSource->GetName().Contains("Build_FloodlightPole")) 
		{
			LightingConnection.mLightSourceType = ELightSourceType::LS_PoleFloodLight;
		}
		else
		{
			LightingConnection.mLightSourceType = ELightSourceType::LS_WallFloodLight;
		}

		LightingConnection.mDistanceToControlPanel = floorf(UpdatedSmartLightingBucket.mControlPanel->GetHorizontalDistanceTo(LightingConnection.mBuildableLightSource)) / 100;
		UpdatedSmartLightingBucket.mBuildableLightingConnections[ConnectionIndex] = LightingConnection;
	}

	if (UpdatedSmartLightingBucket.mBuildableLightingConnections.Num() > 0)
	{
		UpdatedSmartLightingBucket.mBuildableLightingConnections.Sort([](const FBuildableLightingConnection& A, const FBuildableLightingConnection& B)
		{
			if (!A.isConnected) {
				return false;
			}
			else if (!B.isConnected) {
				return true;
			}
			return islessequal(A.mDistanceToControlPanel, B.mDistanceToControlPanel);
		});
	}
	mControlPanelSmartLightingBucket = UpdatedSmartLightingBucket;
}

void ASmartLightsControlPanel::AddBuildableLightSource_Implementation(class AFGBuildableLightSource* LightSource)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddBuildableLightSource"));
	if (LightSource)
	{
		FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
		LightingConnection.mBuildableLightSource = LightSource;
		LightingConnection.mShouldShow = true;
		
		UFGPowerConnectionComponent* LightConnection = Cast<UFGPowerConnectionComponent>(LightingConnection.mBuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
		if (LightConnection) {
			LightingConnection.mBuildablePowerConnection = LightConnection;
		}
		int32 DirtyIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.Add(LightingConnection);
		mControlPanelSmartLightingBucket.mDirtyIndex = DirtyIndex;
	}
}

void ASmartLightsControlPanel::RemoveBuildableLightSource_Implementation(class AFGBuildableLightSource* LightSource)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveBuildableLightSource"));
	if (mControlPanelSmartLightingBucket.mBuildableLightingConnections.Num() > 0) 
	{
		int32 KeyIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.IndexOfByKey(LightSource);
		if (KeyIndex > -1) {
			if (mControlPanelSmartLightingBucket.mBuildableLightingConnections[KeyIndex].mBuildableWire)
			{
				mControlPanelSmartLightingBucket.mBuildableLightingConnections[KeyIndex].mBuildableWire->TurnOffAndDestroy();
				mControlPanelSmartLightingBucket.mBuildableLightingConnections[KeyIndex].mBuildableWire->Destroy(true);
			}
			mControlPanelSmartLightingBucket.mBuildableLightingConnections.RemoveAt(KeyIndex, 1, true);
		}
	}
}

void ASmartLightsControlPanel::AddControlPanelDownstreamConnectionToSmartLightingBucket_Implementation(class UFGCircuitConnectionComponent* DownstreamConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddControlPanelDownstreamConnectionToSmartLightingBucket"));
	mControlPanelSmartLightingBucket.mDownstreamConnection = DownstreamConnection;
}

void ASmartLightsControlPanel::AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel"));
	if (HasAuthority() && mCircuitSubsystem) 
	{
		ABuild_SmartWirelessWireBase* NewBuildableWire = GetWorld()->SpawnActor<ABuild_SmartWirelessWireBase>(ABuild_SmartWirelessWireBase::StaticClass(), FTransform(BuildableLightingConnection.mBuildablePowerConnection->GetRelativeLocation()));
		int32 BuildableConnectionIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
		//mCircuitSubsystem->ConnectComponents(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::AddLightingConnectionToControlPanel power conn CircuitGroupId: %d"), BuildableLightingConnection.mBuildablePowerConnection->GetPowerCircuit()->GetCircuitGroupID());
		NewBuildableWire->Connect(BuildableLightingConnection.mBuildablePowerConnection, Super::mDownstreamConnection);
		BuildableLightingConnection.mBuildableWire = NewBuildableWire;
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(mControlPanelSmartLightingBucket.mControlPanel->IsLightEnabled());
		BuildableLightingConnection.mBuildableLightSource->SetLightControlData(mControlPanelSmartLightingBucket.mControlPanel->GetLightControlData());
		BuildableLightingConnection.isConnected = true;
		BuildableLightingConnection.mShouldShow = true;
		mControlPanelSmartLightingBucket.mDirtyIndex = BuildableConnectionIndex;
		mControlPanelSmartLightingBucket.mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
	}
}

void ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel"));
	
	if (HasAuthority())
	{
		int32 BuildableConnectionIndex = mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
		if (BuildableLightingConnection.mBuildableWire) {
			BuildableLightingConnection.mBuildableWire->Disconnect();
			BuildableLightingConnection.mBuildablePowerConnection->RemoveConnection(BuildableLightingConnection.mBuildableWire);BuildableLightingConnection.mBuildableWire->TurnOffAndDestroy();
			BuildableLightingConnection.mBuildableWire->Destroy(true);
		}
		BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(false);
		BuildableLightingConnection.isConnected = false;
		BuildableLightingConnection.mShouldShow = true;
		BuildableLightingConnection.mBuildableWire = nullptr;
		mControlPanelSmartLightingBucket.mBuildableLightingConnections[BuildableConnectionIndex] = BuildableLightingConnection;
		mControlPanelSmartLightingBucket.mDirtyIndex = BuildableConnectionIndex;
	}
}

void ASmartLightsControlPanel::UpdateLightControlData_Implementation(FLightSourceControlData data)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::UpdateLightControlData"));
	mControlPanelSmartLightingBucket.mControlPanel->SetLightControlData(data);
	for (FBuildableLightingConnection BuildableLightingConnection : mControlPanelSmartLightingBucket.mBuildableLightingConnections) {
		if (BuildableLightingConnection.isConnected) {
			BuildableLightingConnection.mBuildableLightSource->SetLightControlData(data);
		}
	}
}

void ASmartLightsControlPanel::UpdateLightStatus_Implementation(bool LightStatus)
{
	mControlPanelSmartLightingBucket.mControlPanel->SetLightEnabled(LightStatus);
	for (FBuildableLightingConnection BuildableLightingConnection : mControlPanelSmartLightingBucket.mBuildableLightingConnections) {
		if (BuildableLightingConnection.isConnected) {
			BuildableLightingConnection.mBuildableLightSource->SetLightEnabled(LightStatus);
		}
	}
}

FSmartLightingBucket ASmartLightsControlPanel::GetControlPanelSmartLightingBucket()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetControlPanelSmartLightingBucket Num %d"), mControlPanelSmartLightingBucket.mBuildableLightingConnections.Num());
	return mControlPanelSmartLightingBucket;
}

int32 ASmartLightsControlPanel::GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection)
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::RemoveLightingConnectionToControlPanel GetLightingConnectionIndex"));
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections.Find(BuildableLightingConnection);
}

TArray<FBuildableLightingConnection>& ASmartLightsControlPanel::GetBuildableLightingConnections()
{
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections;
}

FBuildableLightingConnection ASmartLightsControlPanel::GetDirtyLightingConnection()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::GetDirtyLightingConnection"));
	return mControlPanelSmartLightingBucket.mBuildableLightingConnections[mControlPanelSmartLightingBucket.mDirtyIndex];
}

void ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated()
{
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanel::OnRep_ControlPanelLightingBucketUpdated"));
}
