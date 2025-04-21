#pragma once
#include "CoreMinimal.h"

class NEXUSHEX_API HexMetrics {
public:
    // Hex direction enumeration
    UENUM()
        enum class EHexDirection : uint8
    {
        N, NE, SE, S, SW, NW
    };

    // Edge type enumeration
    enum class EHexEdgeType { Flat, Slope, Cliff };

    // Edge vertices structure
    struct FEdgeVertices {
        FVector V1, V2, V3, V4, V5;
        FEdgeVertices() : V1(FVector::ZeroVector), V2(FVector::ZeroVector), V3(FVector::ZeroVector),
            V4(FVector::ZeroVector), V5(FVector::ZeroVector) {
        }
        FEdgeVertices(FVector InV1, FVector InV5, float T = 0.0f);
    };

    // Hexagon geometry constants
    static constexpr float OuterRadius = 10.0f;
    static constexpr float InnerRadius = OuterRadius * 0.866025404f;
    static constexpr float SolidFactor = 0.8f;
    static constexpr float ElevationStep = 3.0f;
    static constexpr int32 TerraceSteps = 4;

    static TArray<FVector> Corners;
    static TArray<FColor> NoiseData;
    static UTexture2D* NoiseSource;

    // Initialize corner data
    static void Initialize();
    // Perturb position for natural look
    static FVector Perturb(FVector Position);
    // Get edge vertices between two points
    static FEdgeVertices GetEdgeVertices(FVector V1, FVector V2, float T = 0.0f);
    // Interpolate edge vertices for terracing
    static FEdgeVertices TerraceLerp(FEdgeVertices A, FEdgeVertices B, int32 Step);
    // Interpolate color for terracing
    static FLinearColor TerraceLerp(FLinearColor A, FLinearColor B, int32 Step);
    // Determine edge type based on elevation difference
    static EHexEdgeType GetEdgeType(int32 Elevation1, int32 Elevation2);
    // Get bridge vector for a direction
    static FVector GetBridge(EHexDirection Direction);
    // Get opposite direction
    static EHexDirection Opposite(EHexDirection Direction);
};