#pragma once

#include "Modules/ModuleManager.h"

extern "C" DLLEXPORT IModuleInterface* InitializeModule();

class FSmartWirelessLightingModule : public FDefaultGameModuleImpl {
public:

	virtual bool IsGameModule() const override { return true; }
};