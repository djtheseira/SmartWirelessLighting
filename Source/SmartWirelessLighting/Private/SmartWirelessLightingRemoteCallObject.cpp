#include "SmartWirelessLightingRemoteCallObject.h"

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
#include "SmartLightsControlPanel.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"

USmartWirelessLightingRemoteCallObject::USmartWirelessLightingRemoteCallObject() : Super()
{
	CommandSender = CreateDefaultSubobject<UPlayerCommandSender>(TEXT("PlayerCommandSender"));
}

void USmartWirelessLightingRemoteCallObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, dummy);
	DOREPLIFETIME(USmartWirelessLightingRemoteCallObject, mBuildableLightSources);
}

USmartWirelessLightingRemoteCallObject* USmartWirelessLightingRemoteCallObject::getRCO(UWorld* world) {
	return Cast<USmartWirelessLightingRemoteCallObject>(
		Cast<AFGPlayerController>(world->GetFirstPlayerController()) -> GetRemoteCallObjectOfClass(USmartWirelessLightingRemoteCallObject::StaticClass())
	);
}

USmartWirelessLightingRemoteCallObject* USmartWirelessLightingRemoteCallObject::getRCO(UObject* worldContext) {
	return getRCO(worldContext->GetWorld());
}

void USmartWirelessLightingRemoteCallObject::SetBuildableSubsystem() {
	AFGBuildableSubsystem* buildableSubsystem = AFGBuildableSubsystem::Get(GetWorld());
	if (buildableSubsystem && !mBuildableSubsystem) {
		//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::constructor buildablesubsystem exists"));
		mBuildableSubsystem = buildableSubsystem;
	}
}

void USmartWirelessLightingRemoteCallObject::FindAllBuildableLightSources_Implementation(class ASmartLightsControlPanel* mControlPanel, const TArray< FBuildableLightingConnection>& BuildableLightingConnections)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::FindAllBuildableLightSources"));

	if (mControlPanel->HasAuthority()) {
		//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject2::FindAllBuildableLightSources control panel has authority"));


	}
}

void USmartWirelessLightingRemoteCallObject::SetAvailableLightsForControlPanel_Implementation(class ASmartLightsControlPanel* mControlPanel) {
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::SetAvailableLightsForControlPanel_Implementation mControlPanel exists: %s"), (mControlPanel ? TEXT("YES") : TEXT("NO")));
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::SetAvailableLightsForControlPanel_Implementation mControlPanel hasAuth: %s"), (mControlPanel->HasAuthority() ? TEXT("YES") : TEXT("NO")));
	if (mControlPanel->HasAuthority()) {
		mControlPanel->Server_SetAvailableLightsForControlPanel();
	}
}

void USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO(FString Location)
{
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingRemoteCallObject::DebugWirelessLightingRCO %s"), *Location);
}

void USmartWirelessLightingRemoteCallObject::AddLightConnectionToControlPanel_Implementation(class ASmartLightsControlPanel* ControlPanel, FBuildableLightingConnection BuildableLightingConnection) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_AddLightConnectionToControlPanel(BuildableLightingConnection);
	}
}

void USmartWirelessLightingRemoteCallObject::RemoveLightConnectionFromControlPanel_Implementation(class ASmartLightsControlPanel* ControlPanel, FBuildableLightingConnection BuildableLightingConnection) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_RemoveLightConnectionFromControlPanel(BuildableLightingConnection);
	}
}

void USmartWirelessLightingRemoteCallObject::UpdateControlPanelStatus_Implementation(class ASmartLightsControlPanel* ControlPanel, bool IsEnabled) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateControlPanelStatus(IsEnabled);
	}
}

void USmartWirelessLightingRemoteCallObject::UpdateLightControlData_Implementation(class ASmartLightsControlPanel* ControlPanel, FLightSourceControlData Data) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateLightControlData(Data);
	}
}

void USmartWirelessLightingRemoteCallObject::UpdateLightColorSlot_Implementation(class ASmartLightsControlPanel* ControlPanel, uint8 slotIdx, FLinearColor NewColor) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_UpdateLightColorSlot(slotIdx, NewColor);
	}
}

void USmartWirelessLightingRemoteCallObject::OnDismantleEffectFinishedDestroyConnections_Implementation(class ASmartLightsControlPanel* ControlPanel) {
	if (ControlPanel->HasAuthority()) {
		ControlPanel->Server_OnDismantleEffectFinished();
	}
}