#include "CacilendFPSCharacter.h"
#include "CacilendEnemy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
    UStaticMesh* LoadShape(const TCHAR* Path)
    {
        return LoadObject<UStaticMesh>(nullptr, Path);
    }
    UMaterialInterface* LoadMaterialOrDefault(const FString& Path)
    {
        if (!Path.IsEmpty())
        {
            if (UMaterialInterface* M = LoadObject<UMaterialInterface>(nullptr, *Path)) return M;
        }
        return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }
}

ACacilendFPSCharacter::ACacilendFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(38.0f, 92.0f);
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->MaxWalkSpeed = 430.0f;
    GetCharacterMovement()->MaxWalkSpeedCrouched = 210.0f;
    GetCharacterMovement()->MaxAcceleration = 1400.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1500.0f;
    GetCharacterMovement()->AirControl = 0.25f;
    GetCharacterMovement()->JumpZVelocity = 420.0f;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 66.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->FieldOfView = 92.0f;

    WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
    WeaponRoot->SetupAttachment(FirstPersonCamera);
    WeaponRoot->SetRelativeLocation(FVector(27.0f, 18.0f, -19.0f));

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

UStaticMeshComponent* ACacilendFPSCharacter::AddWeaponPart(FName Name, const TCHAR* MeshPath, const FVector& LocalLocation, const FVector& LocalScale, const FRotator& LocalRotation, const FString& MaterialPath, const FLinearColor& Tint)
{
    UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), Name);
    Part->SetupAttachment(WeaponRoot);
    Part->SetStaticMesh(LoadShape(MeshPath));
    Part->SetRelativeLocation(LocalLocation);
    Part->SetRelativeScale3D(LocalScale);
    Part->SetRelativeRotation(LocalRotation);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Part->CastShadow = false;
    if (UMaterialInterface* Base = LoadMaterialOrDefault(MaterialPath))
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this);
        MID->SetVectorParameterValue(TEXT("Color"), Tint);
        MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
        Part->SetMaterial(0, MID);
    }
    Part->RegisterComponent();
    return Part;
}

void ACacilendFPSCharacter::BeginPlay()
{
    Super::BeginPlay();
    const FString MetalPath = TEXT("/Game/Cacilend/Materials/M_Metal.M_Metal");
    const FString FabricPath = TEXT("/Game/Cacilend/Materials/M_DarkFabric.M_DarkFabric");

    // Prefer a locally imported COD2 weapon mesh when available.
    // If it is absent, keep the original procedural prototype weapon.
    UStaticMesh* ImportedWeapon = LoadShape(TEXT("/Game/Cacilend/Imported/Weapons/SM_COD2_Weapon.SM_COD2_Weapon"));
    if (!ImportedWeapon)
    {
        ImportedWeapon = LoadShape(TEXT("/Game/Cacilend/Imported/Weapons/SM_CC0_Rifle.SM_CC0_Rifle"));
        if (ImportedWeapon)
            UE_LOG(LogTemp, Log, TEXT("[Cacilend] Using optional CC0 rifle fallback; this is not a COD2 original."));
    }
    if (ImportedWeapon)
    {
        UStaticMeshComponent* WeaponMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("ImportedCOD2Weapon"));
        WeaponMesh->SetupAttachment(WeaponRoot);
        WeaponMesh->SetStaticMesh(ImportedWeapon);
        const FVector SourceSize = ImportedWeapon->GetBounds().BoxExtent * 2.0f;
        const float LongestAxis = FMath::Max3(SourceSize.X, SourceSize.Y, SourceSize.Z);
        const float Scale = LongestAxis > 1.0f ? 82.0f / LongestAxis : 1.0f;
        WeaponMesh->SetRelativeScale3D(FVector(Scale));
        WeaponMesh->SetRelativeLocation(FVector(34.0f, 0.0f, -7.0f));
        WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
        WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        WeaponMesh->CastShadow = false;
        WeaponMesh->RegisterComponent();
        UE_LOG(LogTemp, Log, TEXT("[Cacilend] Imported weapon mesh loaded. Orient the FBX barrel toward local +X for best first-person alignment."));
    }
    else
    {
        AddWeaponPart(TEXT("Receiver"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0, 0, 0), FVector(0.29f, 0.065f, 0.080f), FRotator::ZeroRotator, MetalPath, FLinearColor(0.075f,0.085f,0.075f));
        AddWeaponPart(TEXT("Handguard"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(19, 0, 1), FVector(0.20f, 0.057f, 0.061f), FRotator::ZeroRotator, MetalPath, FLinearColor(0.10f,0.11f,0.10f));
        AddWeaponPart(TEXT("Barrel"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), FVector(39, 0, 0), FVector(0.021f,0.021f,0.22f), FRotator(90,0,0), MetalPath, FLinearColor(0.035f,0.04f,0.035f));
        AddWeaponPart(TEXT("Stock"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-25, 0, -1), FVector(0.18f,0.055f,0.075f), FRotator(0,0,-5), FabricPath, FLinearColor(0.10f,0.12f,0.095f));
        AddWeaponPart(TEXT("Magazine"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-1, 0, -10), FVector(0.060f,0.045f,0.13f), FRotator(0,0,-8), MetalPath, FLinearColor(0.06f,0.07f,0.06f));
        AddWeaponPart(TEXT("Grip"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(-7, 0, -11), FVector(0.045f,0.045f,0.085f), FRotator(0,0,-18), FabricPath, FLinearColor(0.12f,0.13f,0.11f));
        AddWeaponPart(TEXT("TopRail"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(8, 0, 5.3f), FVector(0.31f,0.018f,0.010f), FRotator::ZeroRotator, MetalPath, FLinearColor(0.025f,0.03f,0.025f));
        AddWeaponPart(TEXT("FrontSight"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(31, 0, 9.0f), FVector(0.017f,0.018f,0.035f), FRotator::ZeroRotator, MetalPath, FLinearColor(0.025f,0.03f,0.025f));
        AddWeaponPart(TEXT("RightGlove"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(11, 9, -13), FVector(0.095f,0.062f,0.065f), FRotator(0,0,10), FabricPath, FLinearColor(0.17f,0.18f,0.13f));
        AddWeaponPart(TEXT("LeftGlove"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(26, -8, -12), FVector(0.09f,0.06f,0.062f), FRotator(0,0,-10), FabricPath, FLinearColor(0.17f,0.18f,0.13f));
    }
    ConfigurePlayerInputMode();
}

void ACacilendFPSCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    ConfigurePlayerInputMode();
}

void ACacilendFPSCharacter::ConfigurePlayerInputMode()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }
}

void ACacilendFPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    ShotCooldown = FMath::Max(0.0f, ShotCooldown - DeltaSeconds);
    Recoil = FMath::FInterpTo(Recoil, 0.0f, DeltaSeconds, 9.0f);
    SwayTime += DeltaSeconds * (bSprinting ? 11.0f : 6.0f);
    const float Speed2D = GetVelocity().Size2D();
    const float MoveAmount = FMath::Clamp(Speed2D / 450.0f, 0.0f, 1.0f);
    const float SwayY = FMath::Sin(SwayTime) * 0.8f * MoveAmount;
    const float SwayZ = FMath::Abs(FMath::Cos(SwayTime * 0.5f)) * 1.4f * MoveAmount;
    if (WeaponRoot)
    {
        WeaponRoot->SetRelativeLocation(FVector(27.0f - Recoil * 0.5f, 18.0f + SwayY, -19.0f - SwayZ - Recoil));
        WeaponRoot->SetRelativeRotation(FRotator(-Recoil * 0.22f, 0.0f, SwayY * 0.22f));
    }
}

void ACacilendFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACacilendFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACacilendFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ACacilendFPSCharacter::Fire);
    PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ACacilendFPSCharacter::StartReload);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ACacilendFPSCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ACacilendFPSCharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ACacilendFPSCharacter::Interact);
}

void ACacilendFPSCharacter::MoveForward(float Value)
{
    if (Value != 0.0f && Controller)
        AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X), Value);
}

void ACacilendFPSCharacter::MoveRight(float Value)
{
    if (Value != 0.0f && Controller)
        AddMovementInput(FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::Y), Value);
}

void ACacilendFPSCharacter::Fire()
{
    if (bReloading || AmmoInMagazine <= 0 || ShotCooldown > 0.0f || !FirstPersonCamera) return;
    --AmmoInMagazine;
    ShotCooldown = 0.105f;
    Recoil = FMath::Min(10.0f, Recoil + 3.8f);
    const FVector Start = FirstPersonCamera->GetComponentLocation();
    const FVector End = Start + FirstPersonCamera->GetForwardVector() * 16000.0f;
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(CacilendWeaponTrace), true, this);
    Params.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        if (ACacilendEnemy* Enemy = Cast<ACacilendEnemy>(Hit.GetActor()))
        {
            const bool bWasAlive = Enemy->GetHealth() > 0.0f;
            Enemy->TakeDamage(34.0f, FDamageEvent(), GetController(), this);
            if (bWasAlive && Enemy->GetHealth() <= 0.0f) AddKill();
        }
    }
}

void ACacilendFPSCharacter::StartReload()
{
    if (bReloading || AmmoInMagazine >= 30 || ReserveAmmo <= 0) return;
    bReloading = true;
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ACacilendFPSCharacter::FinishReload, 1.45f, false);
}

void ACacilendFPSCharacter::FinishReload()
{
    const int32 Needed = 30 - AmmoInMagazine;
    const int32 Transfer = FMath::Min(Needed, ReserveAmmo);
    AmmoInMagazine += Transfer;
    ReserveAmmo -= Transfer;
    bReloading = false;
}

void ACacilendFPSCharacter::StartSprint()
{
    bSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = 650.0f;
    if (FirstPersonCamera) FirstPersonCamera->SetFieldOfView(99.0f);
}

void ACacilendFPSCharacter::StopSprint()
{
    bSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = 430.0f;
    if (FirstPersonCamera) FirstPersonCamera->SetFieldOfView(92.0f);
}

bool ACacilendFPSCharacter::IsNearAssemblyEntrance() const
{
    if (bInInterior) return true;
    return FVector::DistSquared(GetActorLocation(), FVector(0.0f, 9700.0f, 100.0f)) < FMath::Square(520.0f);
}

void ACacilendFPSCharacter::Interact()
{
    if (!bInInterior)
    {
        const float Dist2 = FVector::DistSquared(GetActorLocation(), FVector(0.0f, 9700.0f, 100.0f));
        if (Dist2 < FMath::Square(600.0f)) ToggleInterior(true);
    }
    else
    {
        const float Dist2 = FVector::DistSquared(GetActorLocation(), FVector(22000.0f, -4600.0f, 100.0f));
        if (Dist2 < FMath::Square(750.0f)) ToggleInterior(false);
    }
}

void ACacilendFPSCharacter::ToggleInterior(bool bEnter)
{
    bInInterior = bEnter;
    if (bEnter)
    {
        ExteriorReturnLocation = FVector(0.0f, 7950.0f, 100.0f);
        SetActorLocation(FVector(22000.0f, -4550.0f, 100.0f), false, nullptr, ETeleportType::TeleportPhysics);
        SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
    }
    else
    {
        SetActorLocation(ExteriorReturnLocation, false, nullptr, ETeleportType::TeleportPhysics);
        SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
    }
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
        PC->SetControlRotation(GetActorRotation());
}

float ACacilendFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health = FMath::Clamp(Health - Applied, 0.0f, 100.0f);
    if (Health <= 0.0f) ResetAfterDeath();
    return Applied;
}

void ACacilendFPSCharacter::ResetAfterDeath()
{
    Health = 100.0f;
    AmmoInMagazine = 30;
    ReserveAmmo = FMath::Max(ReserveAmmo, 60);
    bInInterior = false;
    SetActorLocation(FVector(0.0f, -7200.0f, 100.0f), false, nullptr, ETeleportType::TeleportPhysics);
    SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
    if (APlayerController* PC = Cast<APlayerController>(GetController())) PC->SetControlRotation(GetActorRotation());
}
