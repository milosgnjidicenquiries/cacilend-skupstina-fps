#include "CacilendMapBuilder.h"
#include "CacilendEnemy.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
    UStaticMesh* LoadMeshAsset(const TCHAR* Path)
    {
        return LoadObject<UStaticMesh>(nullptr, Path);
    }
}

ACacilendMapBuilder::ACacilendMapBuilder()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
}

void ACacilendMapBuilder::BeginPlay()
{
    Super::BeginPlay();
    BuildExterior();
    BuildAssemblyInterior();

    const FVector EnemySpawns[] = {
        FVector(-3200, -250, 100), FVector(2800, 700, 100), FVector(-1800, 1800, 100),
        FVector(3400, 2600, 100), FVector(-3600, 3700, 100), FVector(1600, 4300, 100),
        FVector(-900, 5500, 100), FVector(4200, 5500, 100), FVector(-4700, 1500, 100)
    };
    for (const FVector& Pos : EnemySpawns)
        GetWorld()->SpawnActor<ACacilendEnemy>(ACacilendEnemy::StaticClass(), Pos, FRotator(0.0f, 180.0f, 0.0f));
}

UMaterialInterface* ACacilendMapBuilder::ResolveMaterial(const FString& Name)
{
    if (UMaterialInterface** Found = MaterialCache.Find(Name)) return *Found;
    const FString AssetPath = FString::Printf(TEXT("/Game/Cacilend/Materials/M_%s.M_%s"), *Name, *Name);
    UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, *AssetPath);
    if (!Mat) Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    MaterialCache.Add(Name, Mat);
    return Mat;
}

UStaticMeshComponent* ACacilendMapBuilder::AddPrimitive(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation, const FLinearColor& Tint, const TCHAR* MeshPath, bool bCollision, bool bCastShadow)
{
    UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this);
    Component->SetupAttachment(Root);
    Component->SetStaticMesh(LoadMeshAsset(MeshPath));
    Component->SetRelativeLocation(Position);
    Component->SetRelativeRotation(Rotation);
    Component->SetRelativeScale3D(FVector(Dimensions.X / 100.0f, Dimensions.Y / 100.0f, Dimensions.Z / 100.0f));
    Component->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    if (bCollision) Component->SetCollisionProfileName(TEXT("BlockAll"));
    Component->CastShadow = bCastShadow;
    Component->bAffectDistanceFieldLighting = true;
    if (UMaterialInterface* Base = ResolveMaterial(MaterialName))
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this);
        MID->SetVectorParameterValue(TEXT("Tint"), Tint);
        MID->SetVectorParameterValue(TEXT("Color"), Tint);
        MID->SetVectorParameterValue(TEXT("BaseColor"), Tint);
        MID->SetScalarParameterValue(TEXT("UVScale"), FMath::Clamp(Dimensions.GetMax() / 180.0f, 1.0f, 160.0f));
        Component->SetMaterial(0, MID);
    }
    Component->RegisterComponent();
    return Component;
}

UStaticMeshComponent* ACacilendMapBuilder::AddBlock(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation, const FLinearColor& Tint, bool bCollision)
{
    return AddPrimitive(MaterialName, Position, Dimensions, Rotation, Tint, TEXT("/Engine/BasicShapes/Cube.Cube"), bCollision, true);
}

UStaticMeshComponent* ACacilendMapBuilder::AddCylinder(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation, const FLinearColor& Tint, bool bCollision)
{
    return AddPrimitive(MaterialName, Position, Dimensions, Rotation, Tint, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"), bCollision, true);
}

UStaticMeshComponent* ACacilendMapBuilder::AddSphere(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FLinearColor& Tint, bool bCollision)
{
    return AddPrimitive(MaterialName, Position, Dimensions, FRotator::ZeroRotator, Tint, TEXT("/Engine/BasicShapes/Sphere.Sphere"), bCollision, true);
}

void ACacilendMapBuilder::AddTree(float X, float Y, float Height)
{
    const float Trunk = Height * 0.56f;
    AddCylinder(TEXT("Wood"), FVector(X, Y, Trunk * 0.5f), FVector(42, 42, Trunk), FRotator::ZeroRotator, FLinearColor(0.24f,0.16f,0.09f), true);
    const float Crown = Trunk + Height * 0.22f;
    AddSphere(TEXT("Grass"), FVector(X, Y, Crown), FVector(340, 320, 400), FLinearColor(0.18f,0.27f,0.15f), false);
    AddSphere(TEXT("Grass"), FVector(X-100, Y+20, Crown+70), FVector(250,260,280), FLinearColor(0.15f,0.23f,0.13f), false);
    AddSphere(TEXT("Grass"), FVector(X+110, Y-30, Crown+60), FVector(250,245,290), FLinearColor(0.20f,0.30f,0.16f), false);
}

void ACacilendMapBuilder::AddTent(float X, float Y, float Yaw)
{
    const FRotator R(0.0f, Yaw, 0.0f);
    AddBlock(TEXT("DarkFabric"), FVector(X,Y,96), FVector(620,420,190), R, FLinearColor(0.42f,0.39f,0.31f), false);
    AddBlock(TEXT("Canvas"), FVector(X,Y-108,220), FVector(660,350,18), FRotator(0,Yaw,31), FLinearColor(0.83f,0.80f,0.72f), false);
    AddBlock(TEXT("Canvas"), FVector(X,Y+108,220), FVector(660,350,18), FRotator(0,Yaw,-31), FLinearColor(0.78f,0.75f,0.67f), false);
    AddBlock(TEXT("Canvas"), FVector(X,Y-205,118), FVector(600,16,200), R, FLinearColor(0.80f,0.77f,0.69f), false);
    AddBlock(TEXT("Canvas"), FVector(X,Y+205,118), FVector(600,16,200), R, FLinearColor(0.76f,0.73f,0.65f), false);
    for (float Sign : {-1.0f, 1.0f})
    {
        AddCylinder(TEXT("Metal"), FVector(X+Sign*305,Y-192,100), FVector(5,5,200), R, FLinearColor(0.30f,0.30f,0.27f), false);
        AddCylinder(TEXT("Metal"), FVector(X+Sign*305,Y+192,100), FVector(5,5,200), R, FLinearColor(0.30f,0.30f,0.27f), false);
    }
}

void ACacilendMapBuilder::AddBarrier(float X, float Y, float Yaw)
{
    const FRotator R(0.0f, Yaw, 0.0f);
    AddBlock(TEXT("Concrete"), FVector(X,Y,54), FVector(280,95,108), R, FLinearColor(0.52f,0.53f,0.51f), true);
    AddBlock(TEXT("Concrete"), FVector(X,Y,110), FVector(240,74,16), R, FLinearColor(0.60f,0.60f,0.57f), true);
    AddBlock(TEXT("Metal"), FVector(X,Y,136), FVector(265,12,10), R, FLinearColor(0.22f,0.25f,0.22f), false);
}

void ACacilendMapBuilder::AddPallet(float X, float Y, float Z)
{
    AddBlock(TEXT("Wood"), FVector(X,Y,Z+8), FVector(120,95,12), FRotator::ZeroRotator, FLinearColor(0.39f,0.26f,0.14f), true);
    for (int32 I=-2; I<=2; ++I)
        AddBlock(TEXT("Wood"), FVector(X+I*22.0f,Y,Z+19), FVector(15,96,10), FRotator::ZeroRotator, FLinearColor(0.43f,0.30f,0.17f), true);
}

void ACacilendMapBuilder::AddCar(float X, float Y, const FLinearColor& Paint, float Yaw)
{
    const FRotator R(0.0f,Yaw,0.0f);
    AddBlock(TEXT("Metal"), FVector(X,Y,65), FVector(440,190,110), R, Paint, true);
    AddBlock(TEXT("Metal"), FVector(X,Y,124), FVector(235,158,65), R, Paint*0.82f, false);
    AddBlock(TEXT("Glass"), FVector(X,Y,148), FVector(215,146,48), R, FLinearColor(0.12f,0.16f,0.18f), false);
    for (float SX : {-145.0f,145.0f})
        for (float SY : {-94.0f,94.0f})
            AddCylinder(TEXT("DarkFabric"), FVector(X,Y,38)+R.RotateVector(FVector(SX,SY,0)), FVector(58,26,58), R+FRotator(90,0,0), FLinearColor(0.035f,0.04f,0.035f), true);
    AddBlock(TEXT("Metal"), FVector(X+220,Y,68), FVector(14,55,18), R, FLinearColor(0.85f,0.72f,0.48f), false);
    AddBlock(TEXT("Metal"), FVector(X-220,Y,68), FVector(14,55,18), R, FLinearColor(0.70f,0.08f,0.06f), false);
}

void ACacilendMapBuilder::BuildExterior()
{
    AddBlock(TEXT("Asphalt"), FVector(0,900,-32), FVector(16000,24500,70), FRotator::ZeroRotator, FLinearColor::White, true);
    AddBlock(TEXT("Concrete"), FVector(-8500,900,-5), FVector(900,24500,45), FRotator::ZeroRotator, FLinearColor(0.61f,0.60f,0.56f), true);
    AddBlock(TEXT("Concrete"), FVector(8500,900,-5), FVector(900,24500,45), FRotator::ZeroRotator, FLinearColor(0.61f,0.60f,0.56f), true);
    AddBlock(TEXT("Grass"), FVector(-11200,700,-48), FVector(4200,24000,60), FRotator::ZeroRotator, FLinearColor(0.26f,0.31f,0.20f), true);
    AddBlock(TEXT("Grass"), FVector(11200,700,-48), FVector(4200,24000,60), FRotator::ZeroRotator, FLinearColor(0.24f,0.30f,0.19f), true);

    for (int32 I=-5; I<=5; ++I)
        AddBlock(TEXT("Concrete"), FVector(I*1200.0f,-6500,4), FVector(6,5200,2), FRotator::ZeroRotator, FLinearColor(0.38f,0.39f,0.37f), false);
    AddBlock(TEXT("Asphalt"), FVector(-2500,-1400,5), FVector(1500,2700,2), FRotator::ZeroRotator, FLinearColor(0.75f,0.75f,0.72f), false);
    AddBlock(TEXT("Asphalt"), FVector(3000,3600,5), FVector(1200,1700,2), FRotator::ZeroRotator, FLinearColor(0.69f,0.69f,0.66f), false);

    // Recognisable, intentionally prototype-level Assembly facade and approach steps.
    AddBlock(TEXT("Limestone"), FVector(0,10150,950), FVector(11700,360,1900), FRotator::ZeroRotator, FLinearColor(0.78f,0.70f,0.56f), true);
    AddBlock(TEXT("Limestone"), FVector(0,10100,1950), FVector(12300,440,180), FRotator::ZeroRotator, FLinearColor(0.86f,0.79f,0.65f), false);
    AddBlock(TEXT("Limestone"), FVector(0,9850,240), FVector(5100,200,480), FRotator::ZeroRotator, FLinearColor(0.76f,0.67f,0.53f), true);
    for (int32 I=-6; I<=6; ++I)
    {
        const float X=I*430.0f;
        AddCylinder(TEXT("Limestone"), FVector(X,9860,920), FVector(78,78,1450), FRotator::ZeroRotator, FLinearColor(0.91f,0.84f,0.71f), true);
        AddBlock(TEXT("Limestone"), FVector(X,9860,1660), FVector(106,106,34), FRotator::ZeroRotator, FLinearColor(0.90f,0.83f,0.70f), false);
        AddBlock(TEXT("Limestone"), FVector(X,9860,165), FVector(106,106,28), FRotator::ZeroRotator, FLinearColor(0.90f,0.83f,0.70f), false);
    }
    for (int32 Wing : {-1,1})
        for (int32 I=0; I<7; ++I)
        {
            const float X=Wing*(1750.0f+I*650.0f);
            AddBlock(TEXT("Glass"), FVector(X,10335,1070), FVector(290,20,520), FRotator::ZeroRotator, FLinearColor(0.10f,0.13f,0.15f), false);
            AddBlock(TEXT("Limestone"), FVector(X,10315,1345), FVector(340,40,24), FRotator::ZeroRotator, FLinearColor(0.91f,0.84f,0.70f), false);
        }
    for (int32 I=0; I<6; ++I)
        AddBlock(TEXT("Limestone"), FVector(0,8400+I*245.0f,20+I*31.0f), FVector(4400,270,42+I*4), FRotator::ZeroRotator, FLinearColor(0.87f,0.81f,0.69f), true);

    const FVector Tents[] = {
        FVector(-2500,300,0), FVector(0,500,0), FVector(2400,250,0),
        FVector(-3300,1850,0), FVector(-900,2100,0), FVector(1750,1900,0),
        FVector(-2700,3650,0), FVector(500,3650,0), FVector(3000,4000,0),
        FVector(-1400,5150,0), FVector(1500,5400,0)
    };
    for (int32 I=0; I<UE_ARRAY_COUNT(Tents); ++I)
        AddTent(Tents[I].X,Tents[I].Y,(I%3==0)?6.0f:0.0f);
    for (int32 I=0; I<17; ++I)
    {
        const float Y=-300.0f+I*510.0f;
        if (I%2==0) AddBarrier(-4600,Y,90);
        if (I%3!=1) AddBarrier(4550,Y+200,90);
    }
    for (int32 I=0; I<13; ++I) AddPallet(-3900.0f+(I%4)*550.0f,650.0f+I*390.0f);
    for (int32 I=0; I<7; ++I)
    {
        const float X=(I%2==0)?-4900.0f:4900.0f;
        const float Y=-1000.0f+I*1000.0f;
        AddCylinder(TEXT("Metal"), FVector(X,Y,55), FVector(55,55,110), FRotator::ZeroRotator, FLinearColor(0.13f,0.22f,0.13f), true);
        AddCylinder(TEXT("DarkFabric"), FVector(X,Y,112), FVector(48,48,12), FRotator::ZeroRotator, FLinearColor(0.16f,0.25f,0.15f), false);
    }
    AddCar(-6500,-3700,FLinearColor(0.78f,0.77f,0.71f),3);
    AddCar(6500,-2500,FLinearColor(0.12f,0.15f,0.17f),-4);
    AddCar(6600,1800,FLinearColor(0.58f,0.10f,0.08f),2);
    AddCar(-6500,3300,FLinearColor(0.70f,0.71f,0.68f),-2);
    for (int32 I=0; I<10; ++I)
        AddTree((I%2==0)?-9300.0f:9300.0f,-5000.0f+I*2400.0f,520+(I%3)*70.0f);
}

void ACacilendMapBuilder::BuildAssemblyInterior()
{
    const float X=22000.0f;
    AddBlock(TEXT("Carpet"), FVector(X,0,-32), FVector(10200,12600,70), FRotator::ZeroRotator, FLinearColor(0.38f,0.055f,0.065f), true);
    AddBlock(TEXT("InteriorWood"), FVector(X-5100,0,850), FVector(220,12600,1700), FRotator::ZeroRotator, FLinearColor(0.27f,0.15f,0.09f), true);
    AddBlock(TEXT("InteriorWood"), FVector(X+5100,0,850), FVector(220,12600,1700), FRotator::ZeroRotator, FLinearColor(0.27f,0.15f,0.09f), true);
    AddBlock(TEXT("InteriorWood"), FVector(X,6200,850), FVector(10200,220,1700), FRotator::ZeroRotator, FLinearColor(0.25f,0.13f,0.08f), true);
    AddBlock(TEXT("InteriorWood"), FVector(X,0,1760), FVector(10200,12600,120), FRotator::ZeroRotator, FLinearColor(0.20f,0.12f,0.08f), false);
    for (int32 Side : {-1,1})
        for (int32 I=0; I<12; ++I)
        {
            const float Y=-5200.0f+I*930.0f;
            AddBlock(TEXT("Wood"), FVector(X+Side*4970,Y,730), FVector(60,610,850), FRotator::ZeroRotator, FLinearColor(0.34f,0.19f,0.11f), false);
            AddBlock(TEXT("Metal"), FVector(X+Side*4930,Y,1160), FVector(18,620,12), FRotator::ZeroRotator, FLinearColor(0.53f,0.37f,0.15f), false);
        }
    AddBlock(TEXT("Wood"), FVector(X,4350,90), FVector(5000,950,180), FRotator::ZeroRotator, FLinearColor(0.27f,0.14f,0.08f), true);
    AddBlock(TEXT("Carpet"), FVector(X,3800,184), FVector(4600,80,18), FRotator::ZeroRotator, FLinearColor(0.48f,0.08f,0.08f), false);
    AddBlock(TEXT("InteriorWood"), FVector(X,4200,500), FVector(460,240,720), FRotator::ZeroRotator, FLinearColor(0.30f,0.16f,0.09f), true);
    for (int32 Row=0; Row<6; ++Row)
    {
        const float RowY=-2600.0f+Row*1050.0f;
        const int32 Count=Row<2?7:9;
        for (int32 I=0; I<Count; ++I)
        {
            const float Offset=(I-(Count-1)*0.5f)*760.0f;
            const float Curve=FMath::Abs(Offset)*0.10f;
            const FVector Desk(X+Offset,RowY+Curve,420+Row*25.0f);
            AddBlock(TEXT("InteriorWood"), Desk, FVector(550,230,40), FRotator(0,Offset*0.0015f,0), FLinearColor(0.35f,0.20f,0.11f), true);
            AddBlock(TEXT("Wood"), Desk-FVector(0,0,95), FVector(500,185,145), FRotator(0,Offset*0.0015f,0), FLinearColor(0.25f,0.14f,0.08f), true);
            AddBlock(TEXT("Carpet"), FVector(Desk.X,Desk.Y-260,Desk.Z-150), FVector(300,55,240), FRotator(0,Offset*0.0015f,0), FLinearColor(0.29f,0.035f,0.045f), false);
        }
    }
    for (int32 I=0; I<5; ++I)
    {
        const float Y=-4000.0f+I*1900.0f;
        AddCylinder(TEXT("Metal"), FVector(X,Y,1570), FVector(38,38,330), FRotator::ZeroRotator, FLinearColor(0.50f,0.34f,0.15f), false);
        AddSphere(TEXT("Glass"), FVector(X,Y,1410), FVector(310,310,75), FLinearColor(0.95f,0.76f,0.48f), false);
    }
}
