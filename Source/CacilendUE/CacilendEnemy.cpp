#include "CacilendEnemy.h"
#include "CacilendFPSCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
    UStaticMesh* GetMesh(const TCHAR* Path) { return LoadObject<UStaticMesh>(nullptr, Path); }
    UMaterialInterface* GetMat(const FString& Path)
    {
        if (UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, *Path)) return Mat;
        return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }
}

ACacilendEnemy::ACacilendEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(38.0f, 90.0f);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCharacterMovement()->MaxWalkSpeed = 170.0f;
    GetCharacterMovement()->MaxAcceleration = 500.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 500.0f;
}

UStaticMeshComponent* ACacilendEnemy::AddPart(FName Name, const TCHAR* MeshPath, const FVector& Location, const FVector& Scale, const FRotator& Rotation, const FString& MaterialPath, const FLinearColor& Tint)
{
    UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), Name);
    Part->SetupAttachment(GetRootComponent());
    Part->SetStaticMesh(GetMesh(MeshPath));
    Part->SetRelativeLocation(Location);
    Part->SetRelativeScale3D(Scale);
    Part->SetRelativeRotation(Rotation);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Part->CastShadow = true;
    if (UMaterialInterface* Base = GetMat(MaterialPath))
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this);
        MID->SetVectorParameterValue(TEXT("Tint"), Tint);
        MID->SetVectorParameterValue(TEXT("Color"), Tint);
        MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
        Part->SetMaterial(0, MID);
    }
    Part->RegisterComponent();
    return Part;
}

void ACacilendEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Use a local COD2-derived skeletal mesh when the user imports one.
    // A clean checkout deliberately falls back to the built-in proxy soldier.
    if (USkeletalMesh* ImportedSoldier = LoadObject<USkeletalMesh>(nullptr,
            TEXT("/Game/Cacilend/Imported/Characters/SK_COD2_Soldier.SK_COD2_Soldier")))
    {
        if (USkeletalMeshComponent* CharacterMesh = GetMesh())
        {
            CharacterMesh->SetSkeletalMesh(ImportedSoldier);
            CharacterMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
            CharacterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
            const float ImportedHeight = ImportedSoldier->GetBounds().BoxExtent.Z * 2.0f;
            const float CharacterScale = ImportedHeight > 1.0f ? 180.0f / ImportedHeight : 1.0f;
            CharacterMesh->SetRelativeScale3D(FVector(CharacterScale));
            CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            CharacterMesh->SetVisibility(true, true);
            CharacterMesh->SetHiddenInGame(false);
            CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
            ImportedIdleAnimation = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Cacilend/Imported/Animations/AN_COD2_Idle.AN_COD2_Idle"));
            ImportedWalkAnimation = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Cacilend/Imported/Animations/AN_COD2_Walk.AN_COD2_Walk"));
            UAnimSequence* InitialAnimation = ImportedIdleAnimation ? ImportedIdleAnimation.Get() : ImportedWalkAnimation.Get();
            if (InitialAnimation)
            {
                CharacterMesh->PlayAnimation(InitialAnimation, true);
                CurrentImportedAnimation = InitialAnimation;
            }
            bImportedCOD2Mesh = true;
            UE_LOG(LogTemp, Log, TEXT("[Cacilend] Using imported COD2 skeletal soldier; idle=%s walk=%s"),
                ImportedIdleAnimation ? TEXT("yes") : TEXT("no"),
                ImportedWalkAnimation ? TEXT("yes") : TEXT("no"));
        }
        AttackCooldown = FMath::FRandRange(0.3f, 1.4f);
        return;
    }
    const FString Fabric = TEXT("/Game/Cacilend/Materials/M_DarkFabric.M_DarkFabric");
    const FString Metal = TEXT("/Game/Cacilend/Materials/M_Metal.M_Metal");
    const FLinearColor Olive(0.11f, 0.14f, 0.10f);
    AddPart(TEXT("Torso"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0,0,42), FVector(0.30f,0.22f,0.43f), FRotator::ZeroRotator, Fabric, Olive);
    AddPart(TEXT("Vest"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(3,0,43), FVector(0.27f,0.225f,0.29f), FRotator::ZeroRotator, Metal, FLinearColor(0.15f,0.17f,0.13f));
    AddPart(TEXT("Head"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(0,0,83), FVector(0.16f,0.15f,0.17f), FRotator::ZeroRotator, Fabric, FLinearColor(0.32f,0.27f,0.21f));
    AddPart(TEXT("Helmet"), TEXT("/Engine/BasicShapes/Sphere.Sphere"), FVector(0,0,92), FVector(0.18f,0.16f,0.095f), FRotator::ZeroRotator, Fabric, FLinearColor(0.12f,0.15f,0.10f));
    AddPart(TEXT("LeftArm"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(3,-16,46), FVector(0.105f,0.10f,0.30f), FRotator(0,0,-16), Fabric, Olive);
    AddPart(TEXT("RightArm"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(3,16,46), FVector(0.105f,0.10f,0.30f), FRotator(0,0,16), Fabric, Olive);
    AddPart(TEXT("LeftLeg"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0,-7,9), FVector(0.12f,0.095f,0.27f), FRotator::ZeroRotator, Fabric, FLinearColor(0.09f,0.11f,0.08f));
    AddPart(TEXT("RightLeg"), TEXT("/Engine/BasicShapes/Cube.Cube"), FVector(0,7,9), FVector(0.12f,0.095f,0.27f), FRotator::ZeroRotator, Fabric, FLinearColor(0.09f,0.11f,0.08f));
    AttackCooldown = FMath::FRandRange(0.3f, 1.4f);
}

void ACacilendEnemy::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (Health <= 0.0f) return;
    ACacilendFPSCharacter* Player = Cast<ACacilendFPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player || Player->IsInInterior()) return;
    FVector Delta = Player->GetActorLocation() - GetActorLocation();
    Delta.Z = 0.0f;
    const float Dist = Delta.Size();
    const bool bShouldWalk = Dist < 6000.0f && Dist > 240.0f;
    if (bImportedCOD2Mesh)
    {
        UAnimSequence* DesiredAnimation = bShouldWalk && ImportedWalkAnimation
            ? ImportedWalkAnimation.Get()
            : (ImportedIdleAnimation ? ImportedIdleAnimation.Get() : ImportedWalkAnimation.Get());
        if (DesiredAnimation)
        {
            if (USkeletalMeshComponent* CharacterMesh = GetMesh())
            {
                if (CurrentImportedAnimation.Get() != DesiredAnimation)
                {
                    CharacterMesh->PlayAnimation(DesiredAnimation, true);
                    CurrentImportedAnimation = DesiredAnimation;
                }
            }
        }
        bPlayingWalkAnimation = bShouldWalk;
    }
    if (bShouldWalk)
    {
        const FVector Dir = Delta.GetSafeNormal();
        SetActorLocation(GetActorLocation() + Dir * 140.0f * DeltaSeconds, true);
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), Dir.Rotation(), DeltaSeconds, 3.0f));
    }
    AttackCooldown -= DeltaSeconds;
    if (Dist < 1450.0f)
    {
        // Enemies must have a clear sight line; solid buildings/barriers block melee hits.
        FHitResult VisibilityHit;
        FCollisionQueryParams VisibilityParams(SCENE_QUERY_STAT(CacilendEnemyLineOfSight), true, this);
        VisibilityParams.AddIgnoredActor(this);
        VisibilityParams.AddIgnoredActor(Player);
        const FVector EnemyEye = GetActorLocation() + FVector(0.0f, 0.0f, 70.0f);
        const FVector PlayerTarget = Player->GetActorLocation() + FVector(0.0f, 0.0f, 55.0f);
        const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
            VisibilityHit, EnemyEye, PlayerTarget, ECC_Visibility, VisibilityParams);
        if (!bBlocked && AttackCooldown <= 0.0f)
        {
            AttackCooldown = FMath::FRandRange(1.1f, 1.8f);
            Player->TakeDamage(FMath::FRandRange(4.0f, 9.0f), FDamageEvent(), GetController(), this);
        }
    }
}

float ACacilendEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (Health <= 0.0f) return 0.0f;
    const float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health = FMath::Max(0.0f, Health - Applied);
    if (Health <= 0.0f) Destroy();
    return Applied;
}
