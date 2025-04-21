#include "HexCoordinates.h"
#include "Math/UnrealMathUtility.h"

FHexCoordinates FHexCoordinates::FromPosition(FVector Position, int32 GridHeight, float SpacingFactor)
{
    float PosX = Position.X / SpacingFactor;
    float PosY = Position.Y / SpacingFactor;

    float Z = PosX / (HexMetrics::InnerRadius * 1.5f);
    int32 OffsetZ = FMath::RoundToInt(Z);

    float ColOffset = (OffsetZ % 2 == 0) ? 0.0f : HexMetrics::OuterRadius * 0.5f;
    float X = (PosY - ColOffset) / (HexMetrics::OuterRadius * 2.0f);
    int32 OffsetX = FMath::RoundToInt(X);

    int32 OffsetY = -OffsetX - OffsetZ;

    return FHexCoordinates{ OffsetX, OffsetY, OffsetZ };
}

FHexCoordinates FHexCoordinates::FromOffsetCoordinates(int32 OffsetX, int32 OffsetZ)
{
    return FHexCoordinates{ OffsetX, -OffsetX - OffsetZ, OffsetZ };
}

FString FHexCoordinates::ToString(const FHexCoordinates& Coords)
{
    return FString::Printf(TEXT("(%d, %d, %d)"), Coords.X, Coords.Y, Coords.Z);
}

FString FHexCoordinates::ToStringOnSeparateLines(const FHexCoordinates& Coords)
{
    return FString::Printf(TEXT("%d\n%d\n%d"), Coords.X, Coords.Y, Coords.Z);
}