#include "AHexGrid.h"
#include "HexMetrics.h"
#include "HexCoordinates.h"
//#include "HexGridChunk.h"
#include "MassEntitySubsystem.h"
#include "HexGridInitializationSystem.h"

AHexGrid::AHexGrid()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AHexGrid::BeginPlay()
{
    Super::BeginPlay();
    HexMetrics::Initialize();
    HexMetrics::NoiseSource = NoiseSource;

    if (NoiseSource && NoiseSource->GetPlatformData())
    {
        FTexturePlatformData* PlatformData = NoiseSource->GetPlatformData();
        FTexture2DMipMap& Mip = PlatformData->Mips[0];
        FByteBulkData& BulkData = Mip.BulkData;
        uint8* Data = (uint8*)BulkData.LockReadOnly();

        int32 TexWidth = Mip.SizeX;
        int32 TexHeight = Mip.SizeY;
        HexMetrics::NoiseData.SetNum(TexWidth * TexHeight);

        for (int32 Y = 0; Y < TexHeight; Y++)
        {
            for (int32 X = 0; X < TexWidth; X++)
            {
                int32 Index = Y * TexWidth + X;
                HexMetrics::NoiseData[Index] = FColor(Data[Index * 4], Data[Index * 4 + 1], Data[Index * 4 + 2], Data[Index * 4 + 3]);
            }
        }
        BulkData.Unlock();
    }

    MassEntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    CreateChunks();
    InitializeMassEntities();
    Refresh();
}

void AHexGrid::CreateChunks()
{
    if (!ChunkClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ChunkClass is not set!"));
        return;
    }

    Chunks.Empty();
    Chunks.SetNum(ChunkCountX * ChunkCountZ);

    for (int32 Z = 0, I = 0; Z < ChunkCountZ; Z++)
    {
        for (int32 X = 0; X < ChunkCountX; X++)
        {
            AHexGridChunk* Chunk = GetWorld()->SpawnActor<AHexGridChunk>(ChunkClass, FVector::ZeroVector, FRotator::ZeroRotator);
            if (Chunk)
            {
                Chunk->SetActorTransform(GetActorTransform());
                Chunks[I++] = Chunk;
            }
        }
    }
}

void AHexGrid::InitializeMassEntities()
{
    if (MassEntitySubsystem)
    {
        // Trigger initialization system (assumes system is registered in project settings)
        UE_LOG(LogTemp, Log, TEXT("Initializing Mass entities for grid: %d x %d"), GetWidth(), GetHeight());
    }
}

void AHexGrid::Refresh()
{
    for (AHexGridChunk* Chunk : Chunks)
    {
        if (Chunk)
        {
            Chunk->Refresh();
        }
    }
}

AHexCell* AHexGrid::GetCellByPosition(FVector Position)
{
    // Temporary: Convert to Mass entity query
    FVector LocalPosition = GetActorTransform().InverseTransformPosition(Position);
    FHexCoordinates Coords = FHexCoordinates::FromPosition(LocalPosition, GetHeight());
    return GetCellByCoordinates(Coords);
}

AHexCell* AHexGrid::GetCellByCoordinates(FHexCoordinates Coords)
{
    // Temporary: For editor compatibility, return nullptr or adapt to fetch Mass entity data
    return nullptr;
}