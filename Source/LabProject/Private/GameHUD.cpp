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

void AGameHUD::DrawGraph() const
{
	if (!m_graph)
		return;

	const auto& nodes = m_graph->GetNodes();
	auto mapAdjTable = m_graph->GetMapAdjTable();

	const auto& objectColors = m_graph->GetObjectColors();

	const auto& currentPositions = m_graph->GetCurrentPositions();
	const auto& currentLocations = m_graph->GetCurrentLocations();
	const auto& objectPaths = m_graph->GetPaths();

	const float arrowSize = 100.0f * m_graph->GetScale();

	for (int32 i = 0; i < objectPaths.Num(); i++)
	{
		const int32 startJ = objectPaths[i].Find(currentPositions[i]);
		if (startJ == INDEX_NONE)
			continue;
		
		for (int32 j = startJ; j < objectPaths[i].Num() - 1; j++)
		{
			int32 y = objectPaths[i][j].First * nodes[0].Num() + objectPaths[i][j].Second;
			int32 x = objectPaths[i][j + 1].First * nodes[0].Num() + objectPaths[i][j + 1].Second;
	
			if (!mapAdjTable[y].Contains(x))
				continue;
			
			if (objectPaths[i][j] == currentPositions[i])
			{
				GetWorld()->LineBatcher->DrawLine(
					m_graph->GetActorLocation() + nodes[currentPositions[i].First][currentPositions[i].Second],
					m_graph->GetActorLocation() + currentLocations[i],
					FColor::White,
					SDPG_World,
					3.0f * m_graph->GetScale());
				
				GetWorld()->LineBatcher->DrawDirectionalArrow(
					m_graph->GetActorLocation() + currentLocations[i],
					m_graph->GetActorLocation() + nodes[objectPaths[i][j + 1].First][objectPaths[i][j + 1].Second],
					arrowSize,
					objectColors[i % objectColors.Num()],
					0,
					SDPG_World,
					3.0f * m_graph->GetScale());
			}
			else
			{
				GetWorld()->LineBatcher->DrawDirectionalArrow(
					m_graph->GetActorLocation() + nodes[objectPaths[i][j].First][objectPaths[i][j].Second],
					m_graph->GetActorLocation() + nodes[objectPaths[i][j + 1].First][objectPaths[i][j + 1].Second],
					arrowSize,
					objectColors[i % objectColors.Num()],
					0,
					SDPG_World,
					3.0f * m_graph->GetScale());
			}
			
			mapAdjTable[y].Remove(x);
			mapAdjTable[x].Remove(y);
		}
	}
	
	for (const auto& [i, values]: mapAdjTable)
	{
		for (const auto& j: values)
		{
			GetWorld()->LineBatcher->DrawLine(
				m_graph->GetActorLocation() + nodes[i / nodes[0].Num()][i % nodes[0].Num()],
				m_graph->GetActorLocation() + nodes[j / nodes[0].Num()][j % nodes[0].Num()],
				FColor::White,
				SDPG_World,
				3.0f * m_graph->GetScale());
		}
	}
}
