#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CacilendGameMode.generated.h"

UCLASS()
class CACILENDUE_API ACacilendGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACacilendGameMode();
    virtual void BeginPlay() override;
};
