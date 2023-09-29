#include "SmartLightsControlPanelSubsystem.h"

#include "Logging.h"
#include "SmartLightsControlPanel.h"

#include "CoreMinimal.h"
#include "FactoryGame.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "FGCircuitSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerCircuit.h"
#include "FGPowerInfoComponent.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Subsystem/SubsystemActorManager.h"

/**
 * 
 */
ASmartLightsControlPanelSubsystem::ASmartLightsControlPanelSubsystem() {
	//this->ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

void ASmartLightsControlPanelSubsystem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASmartLightsControlPanelSubsystem, mBuildableLightSources);
	DOREPLIFETIME(ASmartLightsControlPanelSubsystem, mBuildableLightingConnections);
}

void ASmartLightsControlPanelSubsystem::BeginPlay() {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::BeginPlay"));
	Super::BeginPlay();
	mBuildableSubsystem = AFGBuildableSubsystem::Get(GetWorld());
	
	/*
	if (mBuildableSubsystem) {
		UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::BeginPlay buildablesubsystem exists"));
		mBuildableSubsystem->BuildableConstructedGlobalDelegate.AddDynamic(this, &ASmartLightsControlPanelSubsystem::RespondToBuildableConstructedGlobal);

	}
	else {
		UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::BeginPlay buildablesubsystem doesnt exist"));
	}
	*/

}

void ASmartLightsControlPanelSubsystem::RespondToBuildableDismanted() {

}

ASmartLightsControlPanelSubsystem* ASmartLightsControlPanelSubsystem::getSubsystem(UWorld* world) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPane lSubsystem::getSubsystem"));
	USubsystemActorManager* SubsystemActorManager = world->GetSubsystem<USubsystemActorManager>();
	check(SubsystemActorManager);

	return SubsystemActorManager->GetSubsystemActor< ASmartLightsControlPanelSubsystem>();
}

void ASmartLightsControlPanelSubsystem::SetLightListIsDirty(bool isLightListDirty) {
	mLightListDirty = isLightListDirty;
}

void ASmartLightsControlPanelSubsystem::RespondToBuildableConstructedGlobal(AFGBuildable* buildable) {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::RespondToBuildableConstructedGlobal RespondToBuildableConstructedGlobal"));

	AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(buildable);

	if (lightSource != nullptr) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::RespondToBuildableConstructedGlobal IsLightSource"));
		lightSource->OnDestroyed.AddDynamic(this, &ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed);
		mBuildableLightSources.AddUnique(lightSource);
		AddNewLightSource(lightSource);
		mLightListDirty = true;
		OnBuildableLightSourceStateChanged.Broadcast();
	}
}

void ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed(AActor* DestroyedActor) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed RespondToLightSourceDestroyed"));

	AFGBuildableLightSource* lightSource = Cast<AFGBuildableLightSource>(DestroyedActor);

	if (lightSource != nullptr) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed actor is light source"));
		mBuildableLightSources.Remove(lightSource);
		RemoveDestroyedLightSource(lightSource);
		mLightListDirty = true;
		OnBuildableLightSourceStateChanged.Broadcast();
	}
}

void ASmartLightsControlPanelSubsystem::OnSmartWirelessLightControlPanelDestroyed() {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::OnSmartWirelessLightControlPanelDestroyed"));
	OnBuildableLightSourceStateChanged.Broadcast();
}

void ASmartLightsControlPanelSubsystem::OnControlPanelToLightConnectionUpdate(class ASmartLightsControlPanel* controlPanel) {
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::OnControlPanelToLightConnectionUpdate"));
	//if (mLightListDirty) return;
	//mLightListDirty = true;
	OnLightSourceStateChanged.Broadcast(controlPanel);
}

void ASmartLightsControlPanelSubsystem::AddNewLightSource(AFGBuildableLightSource* BuildableLightSource) {
	if (HasAuthority()) {
		FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
		ELightSourceType lightSourceType = GetBuildableLightSourceType(BuildableLightSource->GetName());
		UFGPowerConnectionComponent* LightPowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));
		
		// Would be nice to figure out how to tap into the buildeffectfinished stuff..
		//BuildableLightSource->OnBuildEffectFinished()
		
		LightingConnection.mBuildableLightSource = BuildableLightSource;
		LightingConnection.isConnected = false;
		LightingConnection.mDistanceToControlPanel = 0;
		LightingConnection.mBuildablePowerConnection = LightPowerConnection;
		LightingConnection.mLightSourceType = lightSourceType;

		mBuildableLightingConnections.Add(LightingConnection);
	}
}

void ASmartLightsControlPanelSubsystem::RemoveDestroyedLightSource(const AFGBuildableLightSource* LightSource) {
	if (HasAuthority()) {
		auto indexOfLightingConnection = mBuildableLightingConnections.IndexOfByKey(LightSource);
		if (indexOfLightingConnection > -1) {
			mBuildableLightingConnections.RemoveAt(indexOfLightingConnection);
		}
	}
}

void ASmartLightsControlPanelSubsystem::UpdateLightColorSlot(uint8 slotIdx, FLinearColor NewColor) {
	class AFGGameState* FactoryGameState = Cast<AFGGameState>(GetWorld()->GetGameState());
	if (FactoryGameState) {
		FactoryGameState->Server_SetBuildableLightColorSlot(slotIdx, NewColor);
	}
}

void ASmartLightsControlPanelSubsystem::GetAllLightSources() {
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetAllLightSources"));
	// Get All Buildable Light Sources here on init
	if (HasAuthority()) {
		mBuildableLightSources = *(new TArray<AFGBuildableLightSource*>);
		mBuildableSubsystem->GetTypedBuildable(mBuildableLightSources);
		mBuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);

		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetAllLightSources Size: %d"), mBuildableLightSources.Num());
		
		for (AFGBuildableLightSource* BuildableLightSource : mBuildableLightSources) {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Loop Start"));
			FBuildableLightingConnection LightingConnection = FBuildableLightingConnection();
			UFGPowerConnectionComponent* LightPowerConnection = Cast<UFGPowerConnectionComponent>(BuildableLightSource->GetComponentByClass(UFGPowerConnectionComponent::StaticClass()));

			if (!BuildableLightSource->OnDestroyed.Contains(this, "RespondToLightSourceDestroyed")) {
				//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light Doesnt Contain Event"));
				BuildableLightSource->OnDestroyed.AddDynamic(this, &ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed);
			}

			ELightSourceType lightSourceType = GetBuildableLightSourceType(BuildableLightSource->GetName());
			//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetAllLightSources LightSource Intensity: %f"), BuildableLightSource->GetLightControlData().Intensity);

			LightingConnection.mBuildablePowerConnection = LightPowerConnection;
			LightingConnection.mBuildableLightSource = BuildableLightSource;
			LightingConnection.mShouldShow = false;
			LightingConnection.mLightSourceType = lightSourceType;
			LightingConnection.isConnected = false;
			LightingConnection.mDistanceToControlPanel = 0;
			mBuildableLightingConnections.Add(LightingConnection);

			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Loop End"));
		}

		if (mBuildableLightingConnections.Num() > 0)
		{
			mBuildableLightingConnections.Sort([](const FBuildableLightingConnection& A, const FBuildableLightingConnection& B)
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

		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetAllLightSources Size: %d"), mBuildableLightSources.Num());
		//OnBuildableLightSourceStateChanged.Broadcast();
		mLightListDirty = false;
	}

	//TArray< AFGBuildableLightSource* >& Buildables = *(new TArray<AFGBuildableLightSource*>);
}

TArray< FBuildableLightingConnection> ASmartLightsControlPanelSubsystem::GetControlPanelLightSources(ASmartLightsControlPanel* ControlPanel) {
	// Get All Buildable Light Sources since its not actually set.
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources"));
	
	TArray<FBuildableLightingConnection> BuildableLightingConnections = *(new TArray<FBuildableLightingConnection>);
	//if (mLightListDirty) {
		//this->GetAllLightSources();
	//}

	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources: %d"), mBuildableLightSources.Num());
	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Panel GetCircuitID: %d"), ControlPanel->mSmartPanelDownstreamConnection->GetCircuitID());

	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources mLightSources != CP->mLightSourceCount: %d -> %d"), mBuildableLightSources.Num(), ControlPanel->mBuildableLightConnectionCount);

	//if (mBuildableLightSources.Num() != ControlPanel->mBuildableLightConnectionCount) {
	//	ControlPanel->SetBuildableLightConnectionCount(mBuildableLightSources.Num());
	//}

	for (FBuildableLightingConnection LightingConnection : mBuildableLightingConnections) {
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Loop Start"));
		//if (LightingConnection.mLightSourceType != LightSourceFilterType) continue;

		AFGBuildableLightSource* BuildableLightSource = LightingConnection.mBuildableLightSource;
		UFGPowerConnectionComponent* LightPowerConnection = GetLightSourcePowerConnectionComponent(BuildableLightSource);
		LightingConnection.mBuildablePowerConnection = LightPowerConnection;
		
		
		if (!BuildableLightSource->OnDestroyed.Contains(this, "RespondToLightSourceDestroyed")) {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light Doesnt Contain Event"));
			BuildableLightSource->OnDestroyed.AddDynamic(this, &ASmartLightsControlPanelSubsystem::RespondToLightSourceDestroyed);
		}

		if (!LightPowerConnection) {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources !LightPowerConnection"));
			continue;
		}

		TArray< UFGCircuitConnectionComponent* >& LightConnections = *(new TArray< UFGCircuitConnectionComponent*>);
		LightPowerConnection->GetConnections(LightConnections);
		LightPowerConnection->GetHiddenConnections(LightConnections);
		bool IsConnectedToALightControlPanel = IsLightConnectedToLightsControlPanel(LightPowerConnection);
		//bool DoLightConnectionsContainControlPanelConnection = LightPowerConnection->HasHiddenConnection(ControlPanel->mSmartPanelDownstreamConnection);
		bool DoLightConnectionsContainControlPanelConnection = LightConnections.Contains(ControlPanel->mSmartPanelDownstreamConnection);

		

		if ((IsConnectedToALightControlPanel && !DoLightConnectionsContainControlPanelConnection)) {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light MaxConnections: %d"), LightPowerConnection->GetMaxNumConnections());
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light FreeConnections: %d"), LightPowerConnection->GetNumFreeConnections());
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light GetCircuitID: %d"), LightPowerConnection->GetCircuitID());
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light IsConnectedToALightControlPanel: %s"), (IsConnectedToALightControlPanel ? TEXT("true") : TEXT("false")));
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light DoLightConnectionsContainControlPanelConnection: %s"), (DoLightConnectionsContainControlPanelConnection ? TEXT("true") : TEXT("false")));
			continue;
		}
		
		if (DoLightConnectionsContainControlPanelConnection) {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light Connected to Control Panel"));
			LightingConnection.isConnected = true;
			LightingConnection.mBuildableWire = GetControlPanelToLightWire(LightPowerConnection, ControlPanel->mSmartPanelDownstreamConnection);
			BuildableLightSource->SetLightControlData(ControlPanel->GetLightControlData());
		}
		else {
			//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Light Not Connected to Control Panel"));
			LightingConnection.isConnected = false;
		}

		LightingConnection.mDistanceToControlPanel = floorf(ControlPanel->GetHorizontalDistanceTo(LightingConnection.mBuildableLightSource)) / 100;
		BuildableLightingConnections.Add(LightingConnection);
		//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources Loop End"));

	}

	if (BuildableLightingConnections.Num() > 0)
	{
		BuildableLightingConnections.Sort([](const FBuildableLightingConnection& A, const FBuildableLightingConnection& B)
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

	//UE_LOG(LogSWL, Warning, TEXT(".ASmartLightsControlPanelSubsystem::GetControlPanelLightSources: Filtered Connections: %d"), BuildableLightingConnections.Num());
	mLightListDirty = false;
	return BuildableLightingConnections;
}