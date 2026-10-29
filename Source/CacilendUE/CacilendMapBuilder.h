#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CacilendMapBuilder.generated.h"

class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class CACILENDUE_API ACacilendMapBuilder : public AActor
{
    GENERATED_BODY()
public:
    ACacilendMapBuilder();
    virtual void BeginPlay() override;
protected:
    UStaticMeshComponent* AddPrimitive(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation, const FLinearColor& Tint, const TCHAR* MeshPath, bool bCollision, bool bCastShadow = true);
    UStaticMeshComponent* AddBlock(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation = FRotator::ZeroRotator, const FLinearColor& Tint = FLinearColor::White, bool bCollision = true);
    UStaticMeshComponent* AddCylinder(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FRotator& Rotation = FRotator::ZeroRotator, const FLinearColor& Tint = FLinearColor::White, bool bCollision = true);
    UStaticMeshComponent* AddSphere(const FString& MaterialName, const FVector& Position, const FVector& Dimensions, const FLinearColor& Tint = FLinearColor::White, bool bCollision = false);
    UStaticMeshComponent* AddImportedMesh(const TCHAR* AssetPath, const FVector& Position, const FVector& DesiredDimensions, const FRotator& Rotation, bool bCollision = true);
    UMaterialInterface* ResolveMaterial(const FString& Name);
    void BuildExterior();
    void BuildPionirskiPark();
    void BuildPresidency();
    void BuildMainPostOffice();
    void BuildSurroundingBuildings();
    void BuildAssemblyInterior();
    void AddTree(float X, float Y, float Height = 520.0f);
    void AddTent(float X, float Y, float Yaw = 0.0f);
    void AddBarrier(float X, float Y, float Yaw = 0.0f);
    void AddCar(float X, float Y, const FLinearColor& Paint, float Yaw = 0.0f);
    void AddPallet(float X, float Y, float Z = 20.0f);
private:
    UPROPERTY()
    TObjectPtr<USceneComponent> Root;
    TMap<FString, UMaterialInterface*> MaterialCache;
};
