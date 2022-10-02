#include "InitGameWorld.h"

#include "Logging.h"
#include "SmartLightsControlPanelSubsystem.h"
#include "FGBuildableSubsystem.h"
#include "FGPlayerController.h"
#include "FGGameInstance.h"

UInitGameWorld::UInitGameWorld() {
	//UE_LOG(LogSWL, Warning, TEXT(".SmartWirelessLightingUInitGameWorld::UInitGameWorld"));
}

void UInitGameWorld::DispatchLifecycleEvent(ELifecyclePhase Phase) {
	//UE_LOG(LogSWL, Warning, TEXT(".SmartWirelessLightingUInitGameWorld::DispatchLifecycleEvent Phase %s"), *LifecyclePhaseToString(Phase));
	Super::DispatchLifecycleEvent(Phase);
}