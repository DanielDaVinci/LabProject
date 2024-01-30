// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class AGraphPawn;

UCLASS()
class LABPROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void DrawHUD() override;

private:
	UPROPERTY()
	AGraphPawn* m_graph;

	void BindGraphPawn();
	void DrawGraph();
};
