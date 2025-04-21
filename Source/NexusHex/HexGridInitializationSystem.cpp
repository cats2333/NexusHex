#include "HexGridInitializationSystem.h"
#include "MassEntitySubsystem.h"
#include "HexComponents.h"
#include "HexMetrics.h"
#include "HexCoordinates.h"

UHexGridInitializationSystem::UHexGridInitializationSystem()
{
    ExecutionFlags = (int32)EProcessorExecutionFlags::All;
}

void UHexGridInitializationSystem::ConfigureQueries()
{
    GridQuery.AddRequirement<FHexCoordinatesComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexPositionComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexNeighborsComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexElevationComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexColorComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexRoadComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddRequirement<FHexPerturbedCornersComponent>(EMassFragmentAccess::ReadWrite);
    GridQuery.AddTagRequirement<FHexChunkTag>(EMassFragmentPresence::All);
}

void UHexGridInitializationSystem::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    // Assume grid parameters are passed via a shared fragment or configuration
    int32 Width = 10; // Example, replace with actual configuration
    int32 Height = 10;
    float SpacingFactor = 1.0f;

    // Create entities
    for (int32 Z = 0; Z < Height; Z++)
    {
        for (int32 X = 0; X < Width; X++)
        {
            FMassEntityHandle Entity = EntityManager.CreateEntity();

            // Calculate position for pointy-top layout
            float ColOffset = (X % 2 == 0) ? 0.0f : HexMetrics::OuterRadius * 0.5f;
            float PosX = Z * HexMetrics::InnerRadius * 1.5f * SpacingFactor;
            float PosY = (X * HexMetrics::OuterRadius * 2.0f + ColOffset) * SpacingFactor;
            FVector Position(PosX, PosY, 0.0f);

            // Set components
            EntityManager.AddComponent<FHexCoordinatesComponent>(Entity, FHexCoordinatesComponent(X, Z));
            EntityManager.AddComponent<FHexPositionComponent>(Entity, FHexPositionComponent{ Position });
            EntityManager.AddComponent<FHexNeighborsComponent>(Entity, FHexNeighborsComponent());
            EntityManager.AddComponent<FHexElevationComponent>(Entity, FHexElevationComponent{ 0 });
            EntityManager.AddComponent<FHexColorComponent>(Entity, FHexColorComponent{ FLinearColor::White });
            EntityManager.AddComponent<FHexRoadComponent>(Entity, FHexRoadComponent());
            EntityManager.AddComponent<FHexPerturbedCornersComponent>(Entity, FHexPerturbedCornersComponent());
            EntityManager.AddTag<FHexChunkTag>(Entity);

            // Perturb corners
            FHexPerturbedCornersComponent& PerturbedCorners = EntityManager.GetComponent<FHexPerturbedCornersComponent>(Entity);
            for (int32 i = 0; i < 6; i++)
            {
                FVector BaseCorner = HexMetrics::Corners[i];
                FVector WorldCorner = Position + BaseCorner;
                PerturbedCorners.PerturbedCorners[i] = HexMetrics::Perturb(WorldCorner) - Position;
            }
        }
    }

    // Set neighbors
    GridQuery.ForEachEntityChunk(EntityManager, Context, [&](FMassExecutionContext& ChunkContext)
        {
            TArrayView<FHexCoordinatesComponent> Coords = ChunkContext.GetMutableFragmentView<FHexCoordinatesComponent>();
            TArrayView<FHexNeighborsComponent> Neighbors = ChunkContext.GetMutableFragmentView<FHexNeighborsComponent>();

            for (int32 i = 0; i < ChunkContext.GetNumEntities(); i++)
            {
                int32 X = Coords[i].X;
                int32 Z = Coords[i].Z;
                FMassEntityHandle Entity = ChunkContext.GetEntity(i);

                // Find neighbor entities (pointy-top layout)
                if (Z > 0) // NW
                    Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::NW)] = FindNeighborEntity(EntityManager, X, Z - 1);
                if (Z < Width - 1) // SE
                    Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::SE)] = FindNeighborEntity(EntityManager, X, Z + 1);
                if (X > 0)
                {
                    if (X % 2 == 0) // Even row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                    }
                    else // Odd row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                    }
                }
                if (X < Height - 1 && Z < Width - 1)
                    Neighbors[i].Neighbors[static_cast<int32>(EHexDirection::NE)] = FindNeighborEntity(EntityManager, X + 1, Z + 1);
            }
        });
}

FMassEntityHandle FindNeighborEntity(FMassEntityManager& EntityManager, int32 X, int32 Z)
{
    FMassEntityQuery Query;
    Query.AddRequirement<FHexCoordinatesComponent>(EMassFragmentAccess::ReadOnly);
    FMassEntityHandle Result;

    Query.ForEachEntityChunk(EntityManager, [&](FMassExecutionContext& Context)
        {
            TArrayView<FHexCoordinatesComponent> Coords = Context.GetFragmentView<FHexCoordinatesComponent>();
            for (int32 i = 0; i < Context.GetNumEntities(); i++)
            {
                if (Coords[i].X == X && Coords[i].Z == Z)
                {
                    Result = Context.GetEntity(i);
                    break;
                }
            }
        });

    return Result;
}