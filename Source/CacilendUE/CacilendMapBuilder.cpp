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
    BuildPionirskiPark();
    BuildPresidency();
    BuildMainPostOffice();
    BuildSurroundingBuildings();
    BuildAssemblyInterior();

    const FVector EnemySpawns[] = {
        FVector(-15100, 1300, 100), FVector(-11700, 1100, 100), FVector(-9300, 2100, 100),
        FVector(-14500, 3500, 100), FVector(-12100, 3900, 100), FVector(-9700, 4900, 100),
        FVector(-13700, 6400, 100), FVector(-10800, 6900, 100), FVector(-8600, 7800, 100)
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

UStaticMeshComponent* ACacilendMapBuilder::AddImportedMesh(const TCHAR* AssetPath, const FVector& Position, const FVector& DesiredDimensions, const FRotator& Rotation, bool bCollision)
{
    UStaticMesh* Mesh = LoadMeshAsset(AssetPath);
    if (!Mesh) return nullptr;

    const FVector SourceSize = Mesh->GetBounds().BoxExtent * 2.0f;
    const FVector Scale(
        SourceSize.X > 1.0f ? DesiredDimensions.X / SourceSize.X : 1.0f,
        SourceSize.Y > 1.0f ? DesiredDimensions.Y / SourceSize.Y : 1.0f,
        SourceSize.Z > 1.0f ? DesiredDimensions.Z / SourceSize.Z : 1.0f);

    UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this);
    Component->SetupAttachment(Root);
    Component->SetStaticMesh(Mesh);
    Component->SetRelativeLocation(Position);
    Component->SetRelativeRotation(Rotation);
    Component->SetRelativeScale3D(Scale);
    Component->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    if (bCollision) Component->SetCollisionProfileName(TEXT("BlockAll"));
    Component->CastShadow = true;
    Component->bAffectDistanceFieldLighting = true;
    Component->RegisterComponent();
    UE_LOG(LogTemp, Log, TEXT("[Cacilend] Loaded imported mesh: %s"), AssetPath);
    return Component;
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

    // Prefer a local, properly licensed building mesh if one has been imported.
    // The hand-built facade remains as a fallback so a clean clone is playable.
    if (!AddImportedMesh(TEXT("/Game/Cacilend/Imported/Buildings/SM_NationalAssembly.SM_NationalAssembly"),
                         FVector(0,10150,950), FVector(11700,900,2100), FRotator::ZeroRotator, true))
    {
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
    
    
        // Ground-level windows on the approach side (south-facing façade).
        for (int32 Wing : {-1,1})
            for (int32 I=0; I<5; ++I)
            {
                const float X=Wing*(1450.0f+I*760.0f);
                AddBlock(TEXT("Glass"), FVector(X,9964,1080), FVector(350,20,540), FRotator::ZeroRotator, FLinearColor(0.10f,0.13f,0.15f), false);
                AddBlock(TEXT("Limestone"), FVector(X,9950,1370), FVector(390,34,25), FRotator::ZeroRotator, FLinearColor(0.91f,0.84f,0.70f), false);
            }
    }

    const FVector Tents[] = {
        FVector(-13900,900,0), FVector(-12100,800,0), FVector(-10100,1300,0),
        FVector(-14600,2600,0), FVector(-12600,2800,0), FVector(-10500,3300,0),
        FVector(-13900,4500,0), FVector(-11900,5000,0), FVector(-9600,5200,0),
        FVector(-13100,6600,0), FVector(-10900,7000,0)
    };
    for (int32 I=0; I<UE_ARRAY_COUNT(Tents); ++I)
        AddTent(Tents[I].X,Tents[I].Y,(I%3==0)?6.0f:0.0f);

    // Barriers follow the park's west/east edge; they no longer fill the Assembly approach.
    for (int32 I=0; I<15; ++I)
    {
        const float Y=350.0f+I*510.0f;
        if (I%2==0) AddBarrier(-15300,Y,90);
        if (I%3!=1) AddBarrier(-8200,Y+140,90);
    }
    for (int32 I=0; I<13; ++I)
        AddPallet(-14500.0f+(I%4)*420.0f,800.0f+I*430.0f);

    for (int32 I=0; I<6; ++I)
    {
        const float X=(I%2==0)?-14600.0f:-8800.0f;
        const float Y=1100.0f+I*950.0f;
        AddCylinder(TEXT("Metal"), FVector(X,Y,55), FVector(55,55,110), FRotator::ZeroRotator, FLinearColor(0.13f,0.22f,0.13f), true);
        AddCylinder(TEXT("DarkFabric"), FVector(X,Y,112), FVector(48,48,12), FRotator::ZeroRotator, FLinearColor(0.16f,0.25f,0.15f), false);
    }

    // Vehicles are placed along the neighboring streets rather than inside the Assembly facade.
    AddCar(-6500,-3700,FLinearColor(0.78f,0.77f,0.71f),3);
    AddCar(6500,-2500,FLinearColor(0.12f,0.15f,0.17f),-4);
    AddCar(9200,2200,FLinearColor(0.58f,0.10f,0.08f),2);
    AddCar(-17700,3300,FLinearColor(0.70f,0.71f,0.68f),-2);
}

void ACacilendMapBuilder::BuildPionirskiPark()
{
    // Orientation / scale is deliberately explicit: X is west/east and Y is south/north.
    // Exact photogrammetry can be streamed separately with Cesium for Unreal; this is the offline fallback.
    if (AddImportedMesh(TEXT("/Game/Cacilend/Imported/Buildings/SM_PionirskiPark.SM_PionirskiPark"),
                        FVector(-11200,4300,80), FVector(10400,14800,280), FRotator::ZeroRotator, false))
        return;

    const FVector ParkCenter(-11200,4300,0);
    AddBlock(TEXT("Grass"), FVector(ParkCenter.X,ParkCenter.Y,-26), FVector(10400,14800,52),
             FRotator::ZeroRotator, FLinearColor(0.27f,0.34f,0.20f), true);

    // Main walking axes and cross-paths.
    AddBlock(TEXT("Limestone"), FVector(-11200,4300,9), FVector(170,13200,18),
             FRotator::ZeroRotator, FLinearColor(0.67f,0.64f,0.57f), true);
    AddBlock(TEXT("Limestone"), FVector(-11200,4300,10), FVector(9400,170,20),
             FRotator::ZeroRotator, FLinearColor(0.70f,0.67f,0.60f), true);
    AddBlock(TEXT("Limestone"), FVector(-13700,1200,8), FVector(3000,150,16),
             FRotator(0,0,-18), FLinearColor(0.66f,0.63f,0.56f), true);
    AddBlock(TEXT("Limestone"), FVector(-8700,7200,8), FVector(2650,150,16),
             FRotator(0,0,16), FLinearColor(0.66f,0.63f,0.56f), true);

    // Fountain in the southern half of the park, clear of the encampment.
    AddCylinder(TEXT("Limestone"), FVector(-11200,-1250,36), FVector(790,790,72),
                FRotator::ZeroRotator, FLinearColor(0.69f,0.64f,0.55f), true);
    AddCylinder(TEXT("Glass"), FVector(-11200,-1250,76), FVector(660,660,18),
                FRotator::ZeroRotator, FLinearColor(0.18f,0.32f,0.35f), false);
    AddCylinder(TEXT("Limestone"), FVector(-11200,-1250,100), FVector(155,155,46),
                FRotator::ZeroRotator, FLinearColor(0.78f,0.72f,0.62f), false);
    AddSphere(TEXT("Limestone"), FVector(-11200,-1250,145), FVector(150,150,90),
              FLinearColor(0.77f,0.70f,0.59f), false);

    // Curbs mark the perimeter and distinguish lawn from the road.
    AddBlock(TEXT("Limestone"), FVector(-11200,-3100,15), FVector(10500,70,30), FRotator::ZeroRotator, FLinearColor(0.56f,0.54f,0.49f), true);
    AddBlock(TEXT("Limestone"), FVector(-11200,11700,15), FVector(10500,70,30), FRotator::ZeroRotator, FLinearColor(0.56f,0.54f,0.49f), true);
    AddBlock(TEXT("Limestone"), FVector(-16400,4300,15), FVector(70,14800,30), FRotator::ZeroRotator, FLinearColor(0.56f,0.54f,0.49f), true);

    // Low benches face the paths.
    const FVector BenchLocations[] = {
        FVector(-13400,800,150), FVector(-9000,800,150), FVector(-13400,5200,150),
        FVector(-9000,5200,150), FVector(-12800,-500,150), FVector(-9600,-500,150)
    };
    for (const FVector& P : BenchLocations)
    {
        AddBlock(TEXT("Wood"), FVector(P.X,P.Y,P.Z), FVector(190,58,14), FRotator::ZeroRotator, FLinearColor(0.35f,0.23f,0.13f), false);
        AddBlock(TEXT("Wood"), FVector(P.X,P.Y,P.Z+28), FVector(190,24,38), FRotator::ZeroRotator, FLinearColor(0.30f,0.19f,0.11f), false);
        AddBlock(TEXT("Metal"), FVector(P.X-65,P.Y,P.Z-42), FVector(16,45,78), FRotator::ZeroRotator, FLinearColor(0.13f,0.15f,0.13f), false);
        AddBlock(TEXT("Metal"), FVector(P.X+65,P.Y,P.Z-42), FVector(16,45,78), FRotator::ZeroRotator, FLinearColor(0.13f,0.15f,0.13f), false);
    }

    // Mature-tree rows at the park perimeter, with gaps for circulation and sight-lines.
    for (int32 I=0; I<7; ++I)
    {
        const float Y=-1800.0f+I*1950.0f;
        AddTree(-15700.0f,Y,560.0f+(I%3)*55.0f);
        if (I%2==0) AddTree(-6900.0f,Y+350.0f,600.0f+(I%3)*45.0f);
    }
    for (int32 I=0; I<5; ++I)
    {
        const float X=-14900.0f+I*1850.0f;
        AddTree(X,10800.0f,560.0f+(I%2)*55.0f);
        if (I%2==1) AddTree(X,-2550.0f,580.0f);
    }

    // Traditional park lamps: poles and a shallow warm light head.
    for (int32 I=0; I<6; ++I)
    {
        const float Y=-800.0f+I*2050.0f;
        AddCylinder(TEXT("Metal"), FVector(-11950,Y,245), FVector(17,17,490), FRotator::ZeroRotator, FLinearColor(0.09f,0.11f,0.10f), true);
        AddSphere(TEXT("Metal"), FVector(-11950,Y,500), FVector(84,84,40), FLinearColor(0.92f,0.72f,0.40f), false);
    }
}

void ACacilendMapBuilder::BuildPresidency()
{
    // A supplied static mesh can replace this recognizable architectural proxy.
    if (AddImportedMesh(TEXT("/Game/Cacilend/Imported/Buildings/SM_Presidency.SM_Presidency"),
                        FVector(-8700,10900,1040), FVector(3000,2300,2080), FRotator::ZeroRotator, true))
        return;

    const float X=-8700.0f, Y=10900.0f;
    AddBlock(TEXT("Limestone"), FVector(X,Y,70), FVector(3300,2500,140), FRotator::ZeroRotator, FLinearColor(0.59f,0.56f,0.48f), true);
    AddBlock(TEXT("Limestone"), FVector(X,Y,880), FVector(2900,2100,1620), FRotator::ZeroRotator, FLinearColor(0.72f,0.65f,0.53f), true);
    AddBlock(TEXT("Limestone"), FVector(X,Y,1710), FVector(3100,2290,120), FRotator::ZeroRotator, FLinearColor(0.84f,0.76f,0.62f), false);
    AddBlock(TEXT("Limestone"), FVector(X,Y-1040,1280), FVector(960,170,1120), FRotator::ZeroRotator, FLinearColor(0.79f,0.71f,0.57f), false);
    AddBlock(TEXT("Limestone"), FVector(X,Y-1085,1850), FVector(1120,210,90), FRotator::ZeroRotator, FLinearColor(0.86f,0.78f,0.65f), false);

    // Four-storey façade: windows, stone lintels, vertical pilasters and a central entrance.
    for (int32 Floor=0; Floor<4; ++Floor)
    {
        const float Z=370.0f+Floor*365.0f;
        for (int32 Col=-3; Col<=3; ++Col)
        {
            if (Floor==0 && Col==0) continue;
            const float WX=X+Col*365.0f;
            AddBlock(TEXT("Glass"), FVector(WX,Y-1062,Z), FVector(155,22,220), FRotator::ZeroRotator, FLinearColor(0.10f,0.13f,0.15f), false);
            AddBlock(TEXT("Limestone"), FVector(WX,Y-1080,Z+116), FVector(190,34,18), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
            AddBlock(TEXT("Limestone"), FVector(WX,Y-1080,Z-116), FVector(190,34,18), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
        }
    }
    for (int32 I=-2; I<=2; ++I)
    {
        const float CX=X+I*420.0f;
        AddCylinder(TEXT("Limestone"), FVector(CX,Y-1140,800), FVector(70,70,1280), FRotator::ZeroRotator, FLinearColor(0.83f,0.75f,0.63f), true);
        AddBlock(TEXT("Limestone"), FVector(CX,Y-1140,1455), FVector(105,105,35), FRotator::ZeroRotator, FLinearColor(0.87f,0.79f,0.65f), false);
        AddBlock(TEXT("Limestone"), FVector(CX,Y-1140,145), FVector(105,105,35), FRotator::ZeroRotator, FLinearColor(0.87f,0.79f,0.65f), false);
    }
    AddBlock(TEXT("DarkFabric"), FVector(X,Y-1078,260), FVector(210,24,420), FRotator::ZeroRotator, FLinearColor(0.12f,0.10f,0.08f), false);
    AddSphere(TEXT("Limestone"), FVector(X,Y,2110), FVector(920,760,520), FLinearColor(0.53f,0.57f,0.51f), false);
    AddCylinder(TEXT("Metal"), FVector(X,Y,2380), FVector(280,280,190), FRotator::ZeroRotator, FLinearColor(0.28f,0.34f,0.29f), false);
    AddBlock(TEXT("Limestone"), FVector(X,Y,1805), FVector(1050,850,70), FRotator::ZeroRotator, FLinearColor(0.86f,0.78f,0.66f), false);
    AddBlock(TEXT("Limestone"), FVector(X,9730,35), FVector(1800,300,70), FRotator::ZeroRotator, FLinearColor(0.84f,0.76f,0.62f), true);
}

void ACacilendMapBuilder::BuildMainPostOffice()
{
    if (AddImportedMesh(TEXT("/Game/Cacilend/Imported/Buildings/SM_MainPostOfficePalace.SM_MainPostOfficePalace"),
                        FVector(14500,13200,1080), FVector(4200,3200,2160), FRotator::ZeroRotator, true))
        return;

    const float X=14500.0f, Y=13200.0f;
    AddBlock(TEXT("Limestone"), FVector(X,Y,65), FVector(4500,3500,130), FRotator::ZeroRotator, FLinearColor(0.62f,0.58f,0.50f), true);
    AddBlock(TEXT("Limestone"), FVector(X,Y,970), FVector(4100,3100,1800), FRotator::ZeroRotator, FLinearColor(0.70f,0.63f,0.51f), true);
    AddBlock(TEXT("Limestone"), FVector(X,Y,1880), FVector(4300,3300,120), FRotator::ZeroRotator, FLinearColor(0.86f,0.78f,0.63f), false);

    // Monumental west-facing entrance tower.
    const float FrontX=X-2070.0f;
    AddBlock(TEXT("Limestone"), FVector(FrontX-70,Y,1480), FVector(330,950,2860), FRotator::ZeroRotator, FLinearColor(0.78f,0.69f,0.55f), true);
    AddBlock(TEXT("Limestone"), FVector(FrontX-70,Y,2940), FVector(500,1250,150), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
    AddBlock(TEXT("Limestone"), FVector(FrontX-70,Y,2280), FVector(460,1120,120), FRotator::ZeroRotator, FLinearColor(0.86f,0.78f,0.64f), false);

    // Arcaded stone columns and the window rhythm on the west face.
    for (int32 I=-4; I<=4; ++I)
    {
        const float WY=Y+I*330.0f;
        AddCylinder(TEXT("Limestone"), FVector(FrontX-245,WY,1030), FVector(78,78,1380), FRotator::ZeroRotator, FLinearColor(0.83f,0.75f,0.62f), true);
        AddBlock(TEXT("Limestone"), FVector(FrontX-245,WY,1740), FVector(115,115,38), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
        for (int32 Floor=0; Floor<4; ++Floor)
        {
            const float WZ=340.0f+Floor*415.0f;
            AddBlock(TEXT("Glass"), FVector(FrontX+18,WY,WZ), FVector(24,180,235), FRotator::ZeroRotator, FLinearColor(0.10f,0.13f,0.15f), false);
            AddBlock(TEXT("Limestone"), FVector(FrontX+2,WY,WZ+124), FVector(36,210,22), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
        }
    }
    AddBlock(TEXT("DarkFabric"), FVector(FrontX-275,Y,245), FVector(25,420,440), FRotator::ZeroRotator, FLinearColor(0.12f,0.10f,0.08f), false);
    AddSphere(TEXT("Limestone"), FVector(FrontX-70,Y,3110), FVector(580,580,420), FLinearColor(0.74f,0.65f,0.51f), false);
    AddBlock(TEXT("Limestone"), FVector(FrontX-70,Y,3390), FVector(780,780,80), FRotator::ZeroRotator, FLinearColor(0.88f,0.80f,0.66f), false);
}

void ACacilendMapBuilder::BuildSurroundingBuildings()
{
    // Collector roads give the area a recognizable street network beyond the park itself.
    AddBlock(TEXT("Asphalt"), FVector(-18100,6800,-18), FVector(1350,32000,42), FRotator::ZeroRotator, FLinearColor(0.32f,0.33f,0.31f), true);
    AddBlock(TEXT("Asphalt"), FVector(10000,8500,-18), FVector(1350,31000,42), FRotator::ZeroRotator, FLinearColor(0.32f,0.33f,0.31f), true);
    AddBlock(TEXT("Asphalt"), FVector(-5000,20800,-18), FVector(36000,1350,42), FRotator::ZeroRotator, FLinearColor(0.32f,0.33f,0.31f), true);

    auto BuildCityBlock = [this](const FVector& Center, const FVector& Size, int32 Floors, bool bWestFacing)
    {
        const float Height=Floors*335.0f;
        AddBlock(TEXT("Concrete"), FVector(Center.X,Center.Y,45), FVector(Size.X+120,Size.Y+120,90), FRotator::ZeroRotator, FLinearColor(0.55f,0.53f,0.48f), true);
        AddBlock(TEXT("Limestone"), FVector(Center.X,Center.Y,45+Height*0.5f), FVector(Size.X,Size.Y,Height), FRotator::ZeroRotator, FLinearColor(0.63f,0.58f,0.50f), true);
        AddBlock(TEXT("Limestone"), FVector(Center.X,Center.Y,Height+100), FVector(Size.X+140,Size.Y+140,90), FRotator::ZeroRotator, FLinearColor(0.80f,0.73f,0.61f), false);
        for (int32 Floor=0; Floor<Floors; ++Floor)
        {
            const float Z=240.0f+Floor*335.0f;
            if (bWestFacing)
            {
                const float FaceX=Center.X-Size.X*0.5f-14.0f;
                for (int32 Col=0; Col<6; ++Col)
                {
                    const float WY=Center.Y-Size.Y*0.5f+430.0f+Col*560.0f;
                    AddBlock(TEXT("Glass"), FVector(FaceX,WY,Z), FVector(24,180,205), FRotator::ZeroRotator, FLinearColor(0.11f,0.14f,0.16f), false);
                    AddBlock(TEXT("Limestone"), FVector(FaceX-10,WY,Z+110), FVector(36,205,20), FRotator::ZeroRotator, FLinearColor(0.83f,0.75f,0.63f), false);
                }
            }
            else
            {
                const float FaceY=Center.Y-Size.Y*0.5f-14.0f;
                for (int32 Col=0; Col<6; ++Col)
                {
                    const float WX=Center.X-Size.X*0.5f+430.0f+Col*560.0f;
                    AddBlock(TEXT("Glass"), FVector(WX,FaceY,Z), FVector(180,24,205), FRotator::ZeroRotator, FLinearColor(0.11f,0.14f,0.16f), false);
                    AddBlock(TEXT("Limestone"), FVector(WX,FaceY-10,Z+110), FVector(205,36,20), FRotator::ZeroRotator, FLinearColor(0.83f,0.75f,0.63f), false);
                }
            }
        }
    };

    BuildCityBlock(FVector(-22400,-4700,0), FVector(4800,4400,0), 5, true);
    BuildCityBlock(FVector(-22400,2800,0), FVector(4800,4400,0), 4, true);
    BuildCityBlock(FVector(-22400,10500,0), FVector(4800,4400,0), 5, true);
    BuildCityBlock(FVector(-22400,18200,0), FVector(4800,4400,0), 4, true);
    BuildCityBlock(FVector(23000,-4700,0), FVector(5200,4400,0), 5, false);
    BuildCityBlock(FVector(23000,3000,0), FVector(5200,4400,0), 6, false);
    BuildCityBlock(FVector(23000,20200,0), FVector(5200,4400,0), 5, false);
    BuildCityBlock(FVector(-4800,22500,0), FVector(4800,3600,0), 4, false);
    BuildCityBlock(FVector(1800,24500,0), FVector(4800,3600,0), 5, false);
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
