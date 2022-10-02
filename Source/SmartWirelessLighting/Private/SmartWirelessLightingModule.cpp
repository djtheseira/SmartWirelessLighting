#include "SmartWirelessLightingModule.h"

#include "FGGameMode.h"
#include "SmartWirelessLightingRemoteCallObject.h"
#include "FGPlayerController.h"
#include "Patching/NativeHookManager.h"

void FSmartWirelessLightingModule::StartupModule() {
#if !WITH_EDITOR
	AFGGameMode* LocalGameMode = GetMutableDefault<AFGGameMode>();
	SUBSCRIBE_METHOD_VIRTUAL_AFTER(AFGGameMode::PostLogin, (void*)GetDefault<AFGGameMode>(), [](AFGGameMode* gm, APlayerController* pc) {
		if (gm->HasAuthority() && !gm->IsMainMenuGameMode())
		{
			gm->RegisterRemoteCallObjectClass(USmartWirelessLightingRemoteCallObject::StaticClass());
		}
	});

#endif
}

IMPLEMENT_GAME_MODULE(FSmartWirelessLightingModule, SmartWirelessLighting);