#pragma once

#include "Module/GameWorldModule.h"
#include "InitGameWorld.generated.h"


UCLASS()
class SMARTWIRELESSLIGHTING_API UInitGameWorld : public UGameWorldModule {
	
	GENERATED_BODY()

public:

	UInitGameWorld();

	// Begin UGameInstanceModule
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
	// End UGameInstanceModule

};