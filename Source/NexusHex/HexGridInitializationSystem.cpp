#include "HexGridInitializationSystem.h"
#include "MassEntitySubsystem.h"
#include "HexComponents.h"
#include "HexMetrics.h"
#include "MassArchetypeTypes.h"
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

    UMassEntitySubsystem* MassEntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
    if (!MassEntitySubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UMassEntitySubsystem not found!"));
        return;
    }

    // Get the mutable entity manager
    FMassEntityManager& MutableEntityManager = MassEntitySubsystem->GetMutableEntityManager();

    // Define the archetype for the hex grid cells
    TArray<const UStruct*> ArchetypeComponents;
    ArchetypeComponents.Add(FHexCoordinatesComponent::StaticStruct());
    ArchetypeComponents.Add(FHexPositionComponent::StaticStruct());
    ArchetypeComponents.Add(FHexNeighborsComponent::StaticStruct());
    ArchetypeComponents.Add(FHexElevationComponent::StaticStruct());
    ArchetypeComponents.Add(FHexColorComponent::StaticStruct());
    ArchetypeComponents.Add(FHexRoadComponent::StaticStruct());
    ArchetypeComponents.Add(FHexPerturbedCornersComponent::StaticStruct());
    ArchetypeComponents.Add(FHexChunkTag::StaticStruct());

    // Get or create the archetype handle using the entity manager
    FMassArchetypeHandle HexCellArchetype = MutableEntityManager.CreateArchetype(MakeArrayView(ArchetypeComponents));
    // Create entities
    for (int32 Z = 0; Z < Height; Z++)
    {
        for (int32 X = 0; X < Width; X++)
        {
            // Create entity using the archetype handle
            FMassEntityHandle Entity = EntityManager.CreateEntity(HexCellArchetype); // 注意这里只传入了 HexCellArchetype

            // Calculate position for pointy-top layout
            float ColOffset = (X % 2 == 0) ? 0.0f : HexMetrics::OuterRadius * 0.5f;
            float PosX = Z * HexMetrics::InnerRadius * 1.5f * SpacingFactor;
            float PosY = (X * HexMetrics::OuterRadius * 2.0f + ColOffset) * SpacingFactor;
            FVector Position(PosX, PosY, 0.0f);

            // Set components data
            EntityManager.SetComponentData<FHexCoordinatesComponent>(Entity, FHexCoordinatesComponent(X, Z));
            EntityManager.SetComponentData<FHexPositionComponent>(Entity, FHexPositionComponent{ Position });
            EntityManager.SetComponentData<FHexNeighborsComponent>(Entity, FHexNeighborsComponent());
            EntityManager.SetComponentData<FHexElevationComponent>(Entity, FHexElevationComponent{ 0 });
            EntityManager.SetComponentData<FHexColorComponent>(Entity, FHexColorComponent{ FLinearColor::White });
            EntityManager.SetComponentData<FHexRoadComponent>(Entity, FHexRoadComponent());
            EntityManager.SetComponentData<FHexPerturbedCornersComponent>(Entity, FHexPerturbedCornersComponent());
            // Tags don't have data to set
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
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::NW)] = FindNeighborEntity(EntityManager, X, Z - 1);
                if (Z < Width - 1) // SE
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SE)] = FindNeighborEntity(EntityManager, X, Z + 1);
                if (X > 0)
                {
                    if (X % 2 == 0) // Even row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                    }
                    else // Odd row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                    }
                }
                if (X < Height - 1 && Z < Width - 1)
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::NE)] = FindNeighborEntity(EntityManager, X + 1, Z + 1);
            }
        });
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
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::NW)] = FindNeighborEntity(EntityManager, X, Z - 1);
                if (Z < Width - 1) // SE
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SE)] = FindNeighborEntity(EntityManager, X, Z + 1);
                if (X > 0)
                {
                    if (X % 2 == 0) // Even row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                    }
                    else // Odd row
                    {
                        Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::SW)] = FindNeighborEntity(EntityManager, X - 1, Z);
                        if (Z < Width - 1)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::S)] = FindNeighborEntity(EntityManager, X - 1, Z + 1);
                        if (Z > 0)
                            Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::N)] = FindNeighborEntity(EntityManager, X - 1, Z - 1);
                    }
                }
                if (X < Height - 1 && Z < Width - 1)
                    Neighbors[i].Neighbors[static_cast<int32>(HexMetrics::EHexDirection::NE)] = FindNeighborEntity(EntityManager, X + 1, Z + 1);
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