#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CacilendEnemy.generated.h"

class UStaticMeshComponent;

UCLASS()
class CACILENDUE_API ACacilendEnemy : public ACharacter
{
    GENERATED_BODY()
public:
    ACacilendEnemy();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    float GetHealth() const { return Health; }
protected:
    UStaticMeshComponent* AddPart(FName Name, const TCHAR* MeshPath, const FVector& Location, const FVector& Scale, const FRotator& Rotation, const FString& MaterialPath, const FLinearColor& Tint);
private:
    float Health = 100.0f;
    float AttackCooldown = 0.7f;
};
