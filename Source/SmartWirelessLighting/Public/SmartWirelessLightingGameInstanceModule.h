#pragma once
#include "CoreMinimal.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Module/GameInstanceModule.h"
#include "SmartWirelessLightingGameInstanceModule.generated.h"

UCLASS()
class SMARTWIRELESSLIGHTING_API USmartWirelessLightingGameInstanceModule : public UGameInstanceModule 
{
	GENERATED_BODY()

public: 

	// Begin UGameInstanceModule
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
	// End UGameInstanceModule

};