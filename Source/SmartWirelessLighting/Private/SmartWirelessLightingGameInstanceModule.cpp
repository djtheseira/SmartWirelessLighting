#include "SmartWirelessLightingGameInstanceModule.h"
#include "SmartWirelessLightingRemoteCallObject.h"
#include "Registry/RemoteCallObjectRegistry.h"
#include "Logging.h"

void USmartWirelessLightingGameInstanceModule::DispatchLifecycleEvent(ELifecyclePhase Phase) {
	Super::DispatchLifecycleEvent(Phase);
}