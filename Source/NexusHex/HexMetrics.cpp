#include "HexMetrics.h"

// Static member initialization
TArray<FVector> HexMetrics::Corners;
TArray<FColor> HexMetrics::NoiseData;
UTexture2D* HexMetrics::NoiseSource = nullptr;

// Constructor for FEdgeVertices
HexMetrics::FEdgeVertices::FEdgeVertices(FVector InV1, FVector InV5, float T)
{
    V1 = InV1;
    V5 = InV5;
    V3 = FMath::Lerp(InV1, InV5, 0.5f); // Middle point
    V2 = FMath::Lerp(InV1, V3, 0.5f);   // Between V1 and V3
    V4 = FMath::Lerp(V3, V5, 0.5f);     // Between V3 and V5
}

// Initialize hexagon corners for pointy-top layout
void HexMetrics::Initialize()
{
    Corners.SetNum(6);
    Corners[0] = FVector(0.0f, OuterRadius, 0.0f);           // Top
    Corners[1] = FVector(InnerRadius, OuterRadius * 0.5f, 0.0f); // Top-right
    Corners[2] = FVector(InnerRadius, -OuterRadius * 0.5f, 0.0f); // Bottom-right
    Corners[3] = FVector(0.0f, -OuterRadius, 0.0f);          // Bottom
    Corners[4] = FVector(-InnerRadius, -OuterRadius * 0.5f, 0.0f); // Bottom-left
    Corners[5] = FVector(-InnerRadius, OuterRadius * 0.5f, 0.0f); // Top-left
}

// Perturb position (placeholder, to be implemented with noise)
FVector HexMetrics::Perturb(FVector Position)
{
    return Position; // No perturbation for now
}

// Get edge vertices between two points
HexMetrics::FEdgeVertices HexMetrics::GetEdgeVertices(FVector V1, FVector V2, float T)
{
    return FEdgeVertices(V1, V2, T);
}

// Interpolate edge vertices for terracing
HexMetrics::FEdgeVertices HexMetrics::TerraceLerp(FEdgeVertices A, FEdgeVertices B, int32 Step)
{
    FEdgeVertices Result;
    float T = (float)Step / TerraceSteps;
    Result.V1 = FMath::Lerp(A.V1, B.V1, T);
    Result.V2 = FMath::Lerp(A.V2, B.V2, T);
    Result.V3 = FMath::Lerp(A.V3, B.V3, T);
    Result.V4 = FMath::Lerp(A.V4, B.V4, T);
    Result.V5 = FMath::Lerp(A.V5, B.V5, T);
    return Result;
}

// Interpolate color for terracing
FLinearColor HexMetrics::TerraceLerp(FLinearColor A, FLinearColor B, int32 Step)
{
    float T = (float)Step / TerraceSteps;
    return FLinearColor::LerpUsingHSV(A, B, T);
}

// Determine edge type based on elevation difference
HexMetrics::EHexEdgeType HexMetrics::GetEdgeType(int32 Elevation1, int32 Elevation2)
{
    int32 Delta = Elevation2 - Elevation1;
    if (Delta == 0)
        return EHexEdgeType::Flat;
    return (FMath::Abs(Delta) == 1) ? EHexEdgeType::Slope : EHexEdgeType::Cliff;
}

// Get bridge vector for a direction
FVector HexMetrics::GetBridge(EHexDirection Direction)
{
    int32 Index = static_cast<int32>(Direction);
    int32 NextIndex = (Index + 1) % 6;
    return (Corners[Index] + Corners[NextIndex]) * 0.5f;
}

// Get opposite direction
HexMetrics::EHexDirection HexMetrics::Opposite(EHexDirection Direction)
{
    return static_cast<EHexDirection>((static_cast<int32>(Direction) + 3) % 6);
}