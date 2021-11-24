
#include "Build_SmartWirelessWireBase.h"
#include "FGCharacterPlayer.h"
#include "Components/SceneComponent.h"
#include "Hologram/FGWireHologram.h"
#include "Buildables/FGBuildableWire.h"
#include "Components/StaticMeshComponent.h"
//#include "Components/HierarchicalInstancedStaticMeshComponent.h"

ABuild_SmartWirelessWireBase::ABuild_SmartWirelessWireBase() : Super() 
{
	this->bReplicates = true;
	//this->SetReplicates(true);
	this->NetDormancy = DORM_DormantAll;
	this->NetCullDistanceSquared = 5624999936;
	this->mHologramClass = AFGWireHologram::StaticClass();
	//this->mWireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	//this->mDismantleEffectClassName = FSoftClassPath("None");
	//this->mBuildEffectClassName = FSoftClassPath("/Game/FactoryGame/Buildable/Factory/-Shared/BP_MaterialEffect_WireBuild.BP_MaterialEffect_WireBuild_C");
	//this->mWireMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("MainMesh")); this->mWireMesh->SetupAttachment(this->RootComponent);
}

void ABuild_SmartWirelessWireBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
void ABuild_SmartWirelessWireBase::Serialize(FArchive& ar) { Super::Serialize(ar); }
void ABuild_SmartWirelessWireBase::BeginPlay() { Super::BeginPlay(); }
//void ABuild_SmartWirelessWireBase::Destroyed() { }
//void ABuild_SmartWirelessWireBase::Dismantle_Implementation() { }
void ABuild_SmartWirelessWireBase::GetDismantleRefund_Implementation(TArray< FInventoryStack >& out_refund) const { }