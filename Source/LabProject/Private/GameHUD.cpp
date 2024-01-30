// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

#include "GraphPawn.h"
#include "Components/LineBatchComponent.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	BindGraphPawn();
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawGraph();
}

void AGameHUD::BindGraphPawn()
{
	if (!GetWorld())
		return;

	m_graph = Cast<AGraphPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AGameHUD::DrawGraph()
{
	if (!m_graph)
		return;

	const auto& nodes = m_graph->GetNodes();
	auto adjTable = m_graph->GetAdjTable();

	const auto& objectColors = m_graph->GetObjectColors();

	const auto& currentPositions = m_graph->GetCurrentPositions();
	const auto& currentLocations = m_graph->GetCurrentLocations();
	const auto& objectPaths = m_graph->GetPaths();

	const float arrowSize = 100.0f;

	for (int i = 0; i < objectPaths.Num(); i++)
	{
		bool beforeObjectPosition = true;
		for (int j = 0; j < objectPaths[i].Num() - 1; j++)
		{
			if (objectPaths[i][j] != currentPositions[i] && beforeObjectPosition)
			{
				GetWorld()->LineBatcher->DrawDirectionalArrow(
					m_graph->GetActorLocation() + nodes[objectPaths[i][j].First][objectPaths[i][j].Second],
					m_graph->GetActorLocation() + nodes[objectPaths[i][j + 1].First][objectPaths[i][j + 1].Second],
					arrowSize,
					objectColors[i % objectColors.Num()],
					0,
					SDPG_World,
					3.0f);
			}
			else if (objectPaths[i][j] == currentPositions[i])
			{
				beforeObjectPosition = false;
				
				GetWorld()->LineBatcher->DrawDirectionalArrow(
					m_graph->GetActorLocation() + currentLocations[i],
					m_graph->GetActorLocation() + nodes[objectPaths[i][j + 1].First][objectPaths[i][j + 1].Second],
					arrowSize,
					FColor::White,
					0,
					SDPG_World,
					3.0f);

				GetWorld()->LineBatcher->DrawLine(
					m_graph->GetActorLocation() + nodes[currentPositions[i].First][currentPositions[i].Second],
					m_graph->GetActorLocation() + currentLocations[i],
					objectColors[i % objectColors.Num()],
					SDPG_World,
					3.0f);
			}
			else
			{
				GetWorld()->LineBatcher->DrawDirectionalArrow(
					m_graph->GetActorLocation() + nodes[objectPaths[i][j].First][objectPaths[i][j].Second],
					m_graph->GetActorLocation() + nodes[objectPaths[i][j + 1].First][objectPaths[i][j + 1].Second],
					arrowSize,
					FColor::White,
					0,
					SDPG_World,
					3.0f);
			}

			int32 y = objectPaths[i][j].First * nodes[i].Num() + objectPaths[i][j].Second;
			int32 x = objectPaths[i][j + 1].First * nodes[i].Num() + objectPaths[i][j + 1].Second;
			adjTable[y][x] = adjTable[x][y] = 0;
		}
	}

	for (int32 i = 0; i < adjTable.Num(); i++)
	{
		for (int32 j = i + 1; j < adjTable[i].Num(); j++)
		{
			if (adjTable[i][j] == 0)
				continue;

			GetWorld()->LineBatcher->DrawLine(
				m_graph->GetActorLocation() + nodes[i / nodes[0].Num()][i % nodes[0].Num()],
				m_graph->GetActorLocation() + nodes[j / nodes[0].Num()][j % nodes[0].Num()],
				FColor::White,
				SDPG_World,
				3.0f);
		}
	}
}
