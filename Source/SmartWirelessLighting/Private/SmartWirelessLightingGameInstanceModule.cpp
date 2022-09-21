#include "SmartWirelessLightingGameInstanceModule.h"
#include "SmartWirelessLightingRemoteCallObject_New.h"
#include "Registry/RemoteCallObjectRegistry.h"
#include "Logging.h"

void USmartWirelessLightingGameInstanceModule::DispatchLifecycleEvent(ELifecyclePhase Phase) {
	//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingGameInstanceModule::DispatchLifecycleEvent Phase %s"), *LifecyclePhaseToString(Phase));
	if (Phase == ELifecyclePhase::POST_INITIALIZATION) {
		URemoteCallObjectRegistry* RCORegistry = GetGameInstance()->GetSubsystem<URemoteCallObjectRegistry>();
		RCORegistry->RegisterRemoteCallObject(USmartWirelessLightingRemoteCallObject_New::StaticClass()); 
		//UE_LOG(LogSWL, Warning, TEXT(".USmartWirelessLightingGameInstanceModule::LifecycleEvent"));
	}
	Super::DispatchLifecycleEvent(Phase);
}