#include "CacilendHUD.h"
#include "CacilendFPSCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

void ACacilendHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;
    const float W = Canvas->SizeX;
    const float H = Canvas->SizeY;
    const float CX = W * 0.5f;
    const float CY = H * 0.5f;
    const FLinearColor White(0.92f, 0.94f, 0.90f, 0.95f);
    const FLinearColor Green(0.24f, 0.90f, 0.52f, 1.0f);
    const FLinearColor Amber(1.0f, 0.70f, 0.25f, 1.0f);
    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    DrawLine(CX - 10, CY, CX - 4, CY, White, 1.5f);
    DrawLine(CX + 4, CY, CX + 10, CY, White, 1.5f);
    DrawLine(CX, CY - 10, CX, CY - 4, White, 1.5f);
    DrawLine(CX, CY + 4, CX, CY + 10, White, 1.5f);
    DrawLine(CX - 1, CY - 1, CX + 1, CY + 1, FLinearColor(0.0f, 0.0f, 0.0f, 0.8f), 2.5f);
    APlayerController* OwningPC = GetOwningPlayerController();
    ACacilendFPSCharacter* Player = OwningPC ? Cast<ACacilendFPSCharacter>(OwningPC->GetPawn()) : nullptr;
    if (!Player) return;
    if (Player->IsPaused())
    {
        DrawRect(FLinearColor(0.005f, 0.008f, 0.007f, 0.88f), 0.0f, 0.0f, W, H);
        DrawText(TEXT("PAUSED"), White, CX - 55.0f, CY - 42.0f, Font, 2.0f, false);
        DrawText(TEXT("PRESS ESC TO RESUME"), Green, CX - 96.0f, CY + 4.0f, Font, 1.0f, false);
        return;
    }
    DrawText(TEXT("CACILEND // SKUPSTINA"), White, 34.0f, 28.0f, Font, 1.45f, false);
    DrawText(TEXT("MISIJA: OSIGURAJ PRILAZ I UDJI U ZGRADU"), FLinearColor(0.79f, 0.83f, 0.80f, 0.9f), 34.0f, 58.0f, Font, 1.0f, false);
    const int32 PlayerHealth = FMath::Clamp(Player->GetHealth(), 0, 100);
    const FLinearColor HealthColor = PlayerHealth <= 25 ? FLinearColor(1.0f, 0.22f, 0.16f, 1.0f)
        : (PlayerHealth <= 50 ? Amber : Green);
    DrawText(FString::Printf(TEXT("HEALTH  %03d"), PlayerHealth), HealthColor, 36.0f, H - 84.0f, Font, 1.45f, false);
    DrawText(FString::Printf(TEXT("KILLS  %02d"), Player->GetKills()), White, 36.0f, H - 52.0f, Font, 1.0f, false);
    const FString Ammo = Player->IsReloading() ? TEXT("RELOADING...") : FString::Printf(TEXT("%02d  /  %02d"), Player->GetAmmo(), Player->GetReserveAmmo());
    DrawText(Ammo, Player->GetAmmo() <= 5 ? Amber : White, W - 220.0f, H - 82.0f, Font, 1.7f, false);
    DrawText(TEXT("E  INTERACT     R  RELOAD"), FLinearColor(0.75f, 0.79f, 0.76f, 0.75f), W - 260.0f, H - 42.0f, Font, 0.9f, false);
    DrawText(TEXT("WASD KRETANJE  |  SHIFT TRCANJE  |  SPACE SKOK  |  LMB PUCANJE  |  ESC PAUZA"),
        FLinearColor(0.76f, 0.80f, 0.77f, 0.75f), CX - 225.0f, H - 24.0f, Font, 0.8f, false);
    if (Player->IsInInterior())
    {
        DrawText(TEXT("ASSEMBLY CHAMBER"), Amber, CX - 88.0f, 32.0f, Font, 1.0f, false);
        if (Player->IsNearAssemblyEntrance())
            DrawText(TEXT("E  EXIT ASSEMBLY"), Amber, CX - 76.0f, CY + 34.0f, Font, 1.0f, false);
    }
    else if (Player->IsNearAssemblyEntrance())
        DrawText(TEXT("E  ENTER ASSEMBLY"), Amber, CX - 88.0f, CY + 34.0f, Font, 1.0f, false);
}
