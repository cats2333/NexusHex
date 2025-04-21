// Fill out your copyright notice in the Description page of Project Settings.


#pragma once
#include "CoreMinimal.h"
#include "HexMetrics.h"

class NEXUSHEX_API FHexCoordinates {
public:
    static FHexCoordinates FromPosition(FVector Position, int32 GridHeight, float SpacingFactor = 1.0f);
    static FHexCoordinates FromOffsetCoordinates(int32 OffsetX, int32 OffsetZ);
    static FString ToString(const FHexCoordinates& Coords);
    static FString ToStringOnSeparateLines(const FHexCoordinates& Coords);
};