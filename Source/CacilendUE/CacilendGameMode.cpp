#include "CacilendGameMode.h"
#include "CacilendFPSCharacter.h"
#include "CacilendHUD.h"
#include "CacilendMapBuilder.h"
#include "Engine/World.h"
#include "EngineUtils.h"

ACacilendGameMode::ACacilendGameMode()
{
    DefaultPawnClass = ACacilendFPSCharacter::StaticClass();
    HUDClass = ACacilendHUD::StaticClass();
}

void ACacilendGameMode::BeginPlay()
{
    Super::BeginPlay();
    bool bFoundBuilder = false;
    for (TActorIterator<ACacilendMapBuilder> It(GetWorld()); It; ++It)
    {
        bFoundBuilder = true;
        break;
    }
    if (!bFoundBuilder && GetWorld())
    {
        GetWorld()->SpawnActor<ACacilendMapBuilder>(ACacilendMapBuilder::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    }
}
