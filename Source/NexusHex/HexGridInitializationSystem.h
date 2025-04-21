#pragma once
#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "HexGridInitializationSystem.generated.h"

UCLASS()
class NEXUSHEX_API UHexGridInitializationSystem : public UMassProcessor
{
    GENERATED_BODY()

public:
    UHexGridInitializationSystem();

protected:
    virtual void ConfigureQueries() override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery GridQuery;
};