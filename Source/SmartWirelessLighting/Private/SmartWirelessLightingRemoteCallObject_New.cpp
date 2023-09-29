#include "SmartWirelessLightingRemoteCallObject_New.h"

#include "Build_SmartWirelessWireBase.h"
#include "FGBuildableSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGGameState.h"
#include "FGPlayerController.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FGRemoteCallObject.h"
#include "FGWorldSettings.h"
#include "GameFramework/Actor.h"
#include "Hologram/FGWireHologram.h"
#include "Logging.h"
#include "Net/UnrealNetwork.h"
#include "Registry/RemoteCallObjectRegistry.h"
#include "SmartLightsControlPanel_New.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"

USmartWirelessLightingRemoteCallObject_New::USmartWirelessLightingRemoteCallObject_New() : Super()
{
	CommandSender = CreateDefaultSubobject<UPlayerCommandSender>(TEXT("PlayerCommandSender"));
}

void USmartWirelessLightingRemoteCallObject_New::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject_New, dummy);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject_New, mBuildableLightSources);
}

USmartWirelessLightingRemoteCallObject_New* USmartWirelessLightingRemoteCallObject_New::getRCO(UWorld* world) {
	return Cast<USmartWirelessLightingRemoteCallObject_New>(
		Cast<AFGPlayerController>(world->GetFirstPlayerController()) -> GetRemoteCallObjectOfClass(USmartWirelessLightingRemoteCallObject_New::StaticClass())
	);
}

USmartWirelessLightingRemoteCallObject_New* USmartWirelessLightingRemoteCallObject_New::getRCO(UObject* worldContext) {
	return getRCO(worldContext->GetWorld());
}

void USmartWirelessLightingRemoteCallObject_New::SetBuildableSubsystem() {
	AFGBuildableSubsystem* buildableSubsystem = AFGBuildableSubsystem::Get(GetWorld());
	if (buildableSubsystem && !mBuildableSubsystem) {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::constructor buildablesubsystem exists"));
		mBuildableSubsystem = buildableSubsystem;
	}
}

void USmartWirelessLightingRemoteCallObject_New::FindAllBuildableLightSources_Implementation(class ASmartLightsControlPanel_New* mControlPanel, const TArray< FBuildableLightingConnection_New>& BuildableLightingConnections)
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::FindAllBuildableLightSources"));

	if (mControlPanel->HasAuthority()) {
		//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::FindAllBuildableLightSources control panel has authority"));


	}
}

void USmartWirelessLightingRemoteCallObject_New::SetAvailableLightsForControlPanel_Implementation(class ASmartLightsControlPanel_New* mControlPanel) {
	if (mControlPanel->HasAuthority()) {
		mControlPanel->Server_SetAvailableLightsForControlPanel();
	}
}

void USmartWirelessLightingRemoteCallObject_New::DebugWirelessLightingRCO(FString Location)
{
	//UE_LOG(LogSmartWirelessLighting, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO %s"), *Location);
}

void USmartWirelessLightingRemoteCallObject_New::AddLightConnectionToControlPanel_Implementation(class ASmartLightsControlPanel_New* ControlPanel, FBuildableLightingConnection_New BuildableLightingConnection) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_AddLightConnectionToControlPanel(BuildableLightingConnection);
	}
}

void USmartWirelessLightingRemoteCallObject_New::RemoveLightConnectionFromControlPanel_Implementation(class ASmartLightsControlPanel_New* ControlPanel, FBuildableLightingConnection_New BuildableLightingConnection) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_RemoveLightConnectionFromControlPanel(BuildableLightingConnection);
	}
}

void USmartWirelessLightingRemoteCallObject_New::UpdateControlPanelStatus_Implementation(class ASmartLightsControlPanel_New* ControlPanel, bool IsEnabled) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateControlPanelStatus(IsEnabled);
	}
}

void USmartWirelessLightingRemoteCallObject_New::UpdateLightControlData_Implementation(class ASmartLightsControlPanel_New* ControlPanel, FLightSourceControlData Data) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateLightControlData(Data);
	}
}

void USmartWirelessLightingRemoteCallObject_New::UpdateLightColorSlot_Implementation(class ASmartLightsControlPanel_New* ControlPanel, uint8 slotIdx, FLinearColor NewColor) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateLightColorSlot(slotIdx, NewColor);
	}
}

void USmartWirelessLightingRemoteCallObject_New::OnDismantleEffectFinishedDestroyConnections_Implementation(class ASmartLightsControlPanel_New* ControlPanel) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_OnDismantleEffectFinished();
	}
}