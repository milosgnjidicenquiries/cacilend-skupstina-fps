#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "CacilendFPSCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class CACILENDUE_API ACacilendFPSCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ACacilendFPSCharacter();
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    int32 GetAmmo() const { return AmmoInMagazine; }
    int32 GetReserveAmmo() const { return ReserveAmmo; }
    int32 GetHealth() const { return FMath::RoundToInt(Health); }
    int32 GetKills() const { return Kills; }
    bool IsReloading() const { return bReloading; }
    bool IsInInterior() const { return bInInterior; }
    bool IsPaused() const { return bGamePaused; }
    bool IsNearAssemblyInteraction() const;
    void AddKill() { ++Kills; }

protected:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Fire();
    void StartReload();
    void FinishReload();
    void StartSprint();
    void StopSprint();
    void StartCrouch();
    void StopCrouch();
    void Interact();
    void TogglePause();
    void ToggleInterior(bool bEnter);
    void SetPhotogrammetryVisible(bool bVisible);
    void ResetAfterDeath();
    void ConfigurePlayerInputMode();
    UStaticMeshComponent* AddWeaponPart(FName Name, const TCHAR* MeshPath, const FVector& LocalLocation, const FVector& LocalScale, const FRotator& LocalRotation, const FString& MaterialPath, const FLinearColor& Tint);

    UPROPERTY(VisibleAnywhere, Category="Camera")
    TObjectPtr<UCameraComponent> FirstPersonCamera;
    UPROPERTY(VisibleAnywhere, Category="Weapon")
    TObjectPtr<USceneComponent> WeaponRoot;

private:
    int32 AmmoInMagazine = 30;
    int32 ReserveAmmo = 120;
    int32 Kills = 0;
    float Health = 100.0f;
    float Recoil = 0.0f;
    float ShotCooldown = 0.0f;
    float SwayTime = 0.0f;
    bool bReloading = false;
    bool bInInterior = false;
    bool bSprinting = false;
    bool bGamePaused = false;
    FTimerHandle ReloadTimerHandle;
    FVector ExteriorReturnLocation = FVector(0.0f, -7200.0f, 100.0f);
};
