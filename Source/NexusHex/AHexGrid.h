#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHexGrid.generated.h"

class AHexGridChunk;
class UTexture2D;
class UMassEntitySubsystem;

UCLASS()
class NEXUSHEX_API AHexGrid : public AActor
{
    GENERATED_BODY()

public:
    AHexGrid();

    UPROPERTY(EditAnywhere, Category = "HexGrid")
    int32 ChunkCountX = 2;

    UPROPERTY(EditAnywhere, Category = "HexGrid")
    int32 ChunkCountZ = 2;

    UPROPERTY(EditAnywhere, Category = "HexGrid")
    TSubclassOf<AHexGridChunk> ChunkClass;

    UPROPERTY(EditAnywhere, Category = "HexGrid")
    UTexture2D* NoiseSource;

    int32 GetWidth() const { return ChunkCountX * HexMetrics::ChunkSizeX; }
    int32 GetHeight() const { return ChunkCountZ * HexMetrics::ChunkSizeZ; }

    void Refresh();
    AHexCell* GetCellByPosition(FVector Position); // Temporary, for editor compatibility
    AHexCell* GetCellByCoordinates(FHexCoordinates Coordinates); // Temporary, for editor compatibility

protected:
    virtual void BeginPlay() override;

private:
    void CreateChunks();
    void InitializeMassEntities();

    TArray<AHexGridChunk*> Chunks;
    UMassEntitySubsystem* MassEntitySubsystem;
};