#pragma once

#include "CoreMinimal.h"
#include "FGBuildableSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGCircuitSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGInventoryComponent.h"
#include "FGPowerConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "FGUseableInterface.h"
#include "Logging.h"
#include "Buildables/FGBuildable.h"
#include "Buildables/FGBuildableWire.h"
#include "Buildables/FGBuildableLightSource.h"
#include "Buildables/FGBuildableLightsControlPanel.h"
#include "SmartLightsControlPanel.generated.h"

UENUM(BlueprintType)
enum class ELightSourceType : uint8
{
	LS_StreetLight UMETA(DisplayName = "Street Light"),
	LS_CeilingLight UMETA(DisplayName = "Ceiling Light"),
	LS_WallFloodLight UMETA(DisplayName = "Wall Flood Light"),
	LS_PoleFloodLight UMETA(DisplayName = "Pole Flood Light")
};

USTRUCT(Blueprintable)
struct SMARTWIRELESSLIGHTING_API FBuildableLightingConnection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class AFGBuildableLightSource* mBuildableLightSource = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	bool isConnected = false;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class AFGBuildableWire* mBuildableWire = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGCircuitConnectionComponent* mDownstreamConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGPowerConnectionComponent* mBuildablePowerConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	bool mShouldShow = false;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	ELightSourceType mLightSourceType;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	float mDistanceToControlPanel = 0;

	FORCEINLINE bool operator==(const AFGBuildableLightSource* LightSource) const
	{
		return mBuildableLightSource == LightSource;
	}

	FORCEINLINE bool operator==(const FBuildableLightingConnection LightingConnection1) const
	{
		return mBuildableLightSource == LightingConnection1.mBuildableLightSource;
	}

};

USTRUCT(Blueprintable)
struct SMARTWIRELESSLIGHTING_API FSmartLightingBucket
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	TArray<FBuildableLightingConnection> mBuildableLightingConnections;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class ASmartLightsControlPanel* mControlPanel;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	class UFGCircuitConnectionComponent* mDownstreamConnection = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "WirelessLightsControlPanel|Lighting Connections")
	int32 mDirtyIndex;
};

/**
 * 
 */
UCLASS(Blueprintable)
class SMARTWIRELESSLIGHTING_API ASmartLightsControlPanel : public AFGBuildableLightsControlPanel
{
	GENERATED_BODY()

public:
	ASmartLightsControlPanel();

	// Begin Actor Interface
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	// End Actor Interface

	virtual void OnDismantleEffectFinished() override;

	//~ Begin IFGDismantleInterface
	/*virtual void StartIsLookedAtForDismantle_Implementation(class AFGCharacterPlayer* byCharacter) override;
	virtual void GetDismantleRefund_Implementation(TArray< FInventoryStack >& out_refund) const override;
	virtual void PreUpgrade_Implementation() override;*/
	//~ End IFGDismantleInterface

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void SetupVariables(class UFGCircuitConnectionComponent* DownstreamConnection, class ASmartLightsControlPanel* ControlPanel, UPARAM(ref)TArray<FBuildableLightingConnection>& BuildableLightingConnections);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void RefreshControlPanelBucket();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddBuildableLightSource(class AFGBuildableLightSource* LightSource);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void RemoveBuildableLightSource(class AFGBuildableLightSource* LightSource);
	
	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void AddLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(Server, Unreliable)
	void UpdateLightingConnection(FBuildableLightingConnection LightingConnection);

	UFUNCTION(BlueprintCallable, Server, Client, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightControlData(FLightSourceControlData data);

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel")
	void RemoveLightingConnectionToControlPanel(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void UpdateLightStatus(bool LightStatus);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "WirelessLightsControlPanel|LightPanel")
	void SetIsDirtyList();

	UFUNCTION(BlueprintImplementableEvent, Category = "WirelessLightsControlPanel|LightPanel")
	void OnLightingConnectionDestoryed(AActor* destoryedActor);
	
	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnections", CompactNodeTitle = "BuildableLightingConnections" ))
	TArray<FBuildableLightingConnection>& GetBuildableLightingConnections();

	UFUNCTION(BlueprintCallable, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetDirtyLightingConnection"))
	FBuildableLightingConnection GetDirtyLightingConnection();
	
	UFUNCTION(BlueprintPure, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "GetLightingConnectionIndex", CompactNodeTitle = "ConnectionIndex" ))
	int32 GetLightingConnectionIndex(FBuildableLightingConnection BuildableLightingConnection);

	UFUNCTION(BlueprintPure, Category = "WirelessLightsControlPanel|LightPanel", meta = (DisplayName = "IsDirtyList", CompactNodeTitle = "IsDirtyList"))
	bool GetIsDirtyList();


protected:

	UFUNCTION()
	void OnRep_ControlPanelBuildableLightingConnections();

	UFUNCTION()
	void OnRep_ControlPanel();
	
	UFUNCTION()
	void OnRep_DirtyIndex();

	UFUNCTION()
	void OnRep_IsDirtyList();
	
private:

	UPROPERTY(Replicated)
	bool mIsFirstUpdate = true;

	UPROPERTY(ReplicatedUsing = "OnRep_ControlPanelBuildableLightingConnections")
	TArray<FBuildableLightingConnection> mBuildableLightingConnections;

	UPROPERTY(ReplicatedUsing = "OnRep_ControlPanel")
	class ASmartLightsControlPanel* mControlPanel;

	UPROPERTY(ReplicatedUsing = "OnRep_DirtyIndex")
	int32 mDirtyIndex;

	UPROPERTY(ReplicatedUsing = "OnRep_IsDirtyList")
	bool mIsDirtyList;
	
	class AFGCircuitSubsystem* mCircuitSubsystem;
	

private:

	AFGBuildableWire* GetControlPanelToLightWire(class UFGPowerConnectionComponent* PowerConnection) 
	{
		TArray< class AFGBuildableWire* >& ConnectedWires = *(new TArray<class AFGBuildableWire*>);
		PowerConnection->GetWires(ConnectedWires);
		for (AFGBuildableWire* Wire : ConnectedWires) {
			UFGCircuitConnectionComponent* OppositeCircuitConnection = Wire->GetOppositeConnection(PowerConnection);
			if (OppositeCircuitConnection && OppositeCircuitConnection == this->mDownstreamConnection) {
				return Wire;
			}
		}
		return nullptr;
	}

	ELightSourceType GetBuildableLightSourceType(FString LightSourceName)
	{
		if (LightSourceName.Contains("Build_StreetLight"))
		{
			return ELightSourceType::LS_StreetLight;
		}
		else if (LightSourceName.Contains("Build_CeilingLight"))
		{
			return ELightSourceType::LS_CeilingLight;
		}
		else if (LightSourceName.Contains("Build_FloodlightPole"))
		{
			return ELightSourceType::LS_PoleFloodLight;
		}
		else
		{
			return ELightSourceType::LS_WallFloodLight;
		}
	}

};
