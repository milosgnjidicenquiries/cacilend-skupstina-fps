#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CacilendHUD.generated.h"

UCLASS()
class CACILENDUE_API ACacilendHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
