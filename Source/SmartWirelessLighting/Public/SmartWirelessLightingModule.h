#pragma once

#include "Modules/ModuleManager.h"

extern "C" DLLEXPORT IModuleInterface* InitializeModule();

class FSmartWirelessLightingModule : public FDefaultGameModuleImpl {
public:
	virtual void StartupModule() override;

	virtual bool IsGameModule() const override { return true; }
};