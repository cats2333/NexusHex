#pragma once
#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "HexMetrics.h"
#include "HexComponents.generated.h"

USTRUCT()
struct FHexCoordinatesComponent : public FMassSharedFragment
{
    GENERATED_BODY()
    int32 X;
    int32 Y;
    int32 Z;

    FHexCoordinatesComponent() : X(0), Y(0), Z(0) {}
    FHexCoordinatesComponent(int32 InX, int32 InZ) : X(InX), Z(InZ), Y(-InX - InZ) {}
};

USTRUCT()
struct FHexPositionComponent : public FMassFragment
{
    GENERATED_BODY()
    FVector Position;

    FHexPositionComponent() : Position(FVector::ZeroVector) {}
};

USTRUCT()
struct FHexNeighborsComponent : public FMassFragment
{
    GENERATED_BODY()
    TArray<FMassEntityHandle> Neighbors; // Stores entity handles for 6 directions

    FHexNeighborsComponent()
    {
        Neighbors.Init(FMassEntityHandle(), 6); // Matches EHexDirection count
    }
};

USTRUCT()
struct FHexElevationComponent : public FMassFragment
{
    GENERATED_BODY()
    int32 Elevation;

    FHexElevationComponent() : Elevation(0) {}
};

USTRUCT()
struct FHexColorComponent : public FMassFragment
{
    GENERATED_BODY()
    FLinearColor Color;

    FHexColorComponent() : Color(FLinearColor::White) {}
};

USTRUCT()
struct FHexRoadComponent : public FMassFragment
{
    GENERATED_BODY()
    bool bHasOutgoingRoad;
    bool bHasIncomingRoad;
    HexMetrics::EHexDirection OutgoingRoad;
    HexMetrics::EHexDirection IncomingRoad;

    FHexRoadComponent() : bHasOutgoingRoad(false), bHasIncomingRoad(false),
        OutgoingRoad(HexMetrics::EHexDirection::N), IncomingRoad(HexMetrics::EHexDirection::N) {
    }
};

USTRUCT()
struct FHexHighlightComponent : public FMassFragment
{
    GENERATED_BODY()
    bool bIsHighlighted;

    FHexHighlightComponent() : bIsHighlighted(false) {}
};

USTRUCT()
struct FHexPerturbedCornersComponent : public FMassFragment
{
    GENERATED_BODY()
    TArray<FVector> PerturbedCorners;

    FHexPerturbedCornersComponent()
    {
        PerturbedCorners.SetNum(6);
    }
};

USTRUCT()
struct FHexChunkTag : public FMassTag
{
    GENERATED_BODY()
};
