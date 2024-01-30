#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.generated.h"



USTRUCT(BlueprintType)
struct FGraphParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 graphPointsCountX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 graphPointsCountY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float graphStep = 100.0f;
	
};

USTRUCT(BlueprintType)
struct FCameraSensitivity
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera")
	float mouseMoveSensitivity = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera")
	float mouseWheelSensitivity = 35.0f;
};
