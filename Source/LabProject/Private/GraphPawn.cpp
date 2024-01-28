// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphPawn.h"

#include "GraphPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

AGraphPawn::AGraphPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	pSceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(pSceneComponent);

	pSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	pSpringArmComponent->SetupAttachment(GetRootComponent());

	pCameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	pCameraComponent->SetupAttachment(pSpringArmComponent);

	pInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(
		"InstancedStaticMeshComponent");
	pInstancedStaticMeshComponent->SetupAttachment(GetRootComponent());
	pInstancedStaticMeshComponent->NumCustomDataFloats = 3;
}

void AGraphPawn::BeginPlay()
{
	Super::BeginPlay();

	InputBind();
	InitGraph();
}

void AGraphPawn::InputBind()
{
	AGraphPlayerController* playerController = Cast<AGraphPlayerController>(GetController());
	if (!playerController)
		return;

	playerController->OnMouseDown.BindUObject(this, &AGraphPawn::OnMouseDown);
	playerController->OnMousePressedMove.BindUObject(this, &AGraphPawn::OnMouseMove);
}

void AGraphPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MouseInputRotate(DeltaTime);
	MoveObjects(DeltaTime);
}

void AGraphPawn::OnMouseDown(FVector2D MousePosition)
{
	m_startMousePosition = MousePosition;
	m_currentMousePosition = MousePosition;
	m_startCameraRotation = pSpringArmComponent->GetRelativeRotation();
}

void AGraphPawn::OnMouseMove(FVector2D MousePosition)
{
	m_currentMousePosition = MousePosition;
}

void AGraphPawn::MouseInputRotate(float DeltaTime) const
{
	const FVector2D mouseDelta = m_currentMousePosition - m_startMousePosition;
	const FRotator deltaRotation = FRotator(-1 * mouseDelta.Y * cameraSensitivity, mouseDelta.X * cameraSensitivity,
	                                        0.0f);

	FRotator currentRotation = FMath::RInterpTo(
		pSpringArmComponent->GetRelativeRotation(),
		m_startCameraRotation + deltaRotation,
		DeltaTime, 10);
	currentRotation.Pitch = FMath::Clamp(currentRotation.Pitch, -89.0f, 89.0f);

	pSpringArmComponent->SetRelativeRotation(currentRotation);
}

void AGraphPawn::InitGraph()
{
	InitNodes();
	InitAdjTable();

	InitObjects();
	InitPaths();
}

void AGraphPawn::InitNodes()
{
	TArray<FVector> exp;
	exp.Init(FVector::ZeroVector, graphPointsCountX);
	m_nodes.Init(exp, graphPointsCountY);

	FVector centerOffset(-1 * graphPointsCountX * graphStep / 2.0f, -1 * graphPointsCountY * graphStep / 2.0f, 0.0f);

	for (int32 i = 0; i < graphPointsCountY; i++)
	{
		for (int32 j = 0; j < graphPointsCountX; j++)
		{
			FVector offset = {
				FMath::FRandRange(-1 * graphStep / 2.0f, graphStep / 2.0f),
				FMath::FRandRange(-1 * graphStep / 2.0f, graphStep / 2.0f),
				FMath::FRandRange(-1 * graphStep / 2.0f, graphStep / 2.0f)
			};

			m_nodes[i][j] = centerOffset + FVector(j * graphStep, i * graphStep, 0.0f) + offset;
		}
	}
}

void AGraphPawn::InitAdjTable()
{
	TArray<int32> exp;
	exp.Init(0, graphPointsCountY * graphPointsCountX);
	m_adjTable.Init(exp, graphPointsCountY * graphPointsCountX);

	for (int32 i = 0; i < graphPointsCountY; i++)
	{
		for (int32 j = 0; j < graphPointsCountX; j++)
		{
			int32 y = i * graphPointsCountX + j;

			if (i + 1 < graphPointsCountY)
			{
				int32 x = (i + 1) * graphPointsCountX + j;
				m_adjTable[y][x] = m_adjTable[x][y] = 1;
			}
			if (j + 1 < graphPointsCountX)
			{
				int32 x = i * graphPointsCountX + j + 1;
				m_adjTable[y][x] = m_adjTable[x][y] = 1;
			}
		}
	}
}

TArray<FVector> AGraphPawn::GetCurrentLocations() const
{
	TArray<FVector> locations;
	for (int i = 0; i < objectCount; i++)
	{
		FTransform transform;
		pInstancedStaticMeshComponent->GetInstanceTransform(i, transform);

		locations.Add(transform.GetLocation());
	}

	return locations;
}

void AGraphPawn::InitObjects()
{
	for (int32 i = 0; i < objectCount; i++)
	{
		m_currentPositions.Add(Chaos::Pair(
			FMath::RandRange(0, graphPointsCountY - 1),
			FMath::RandRange(0, graphPointsCountX - 1)));

		FVector position = m_nodes[m_currentPositions[i].First][m_currentPositions[i].Second];

		pInstancedStaticMeshComponent->AddInstance(FTransform(position));
		pInstancedStaticMeshComponent->SetCustomDataValue(i, 0, objectColors[i % objectColors.Num()].R);
		pInstancedStaticMeshComponent->SetCustomDataValue(i, 1, objectColors[i % objectColors.Num()].G);
		pInstancedStaticMeshComponent->SetCustomDataValue(i, 2, objectColors[i % objectColors.Num()].B);
	}
}

void AGraphPawn::InitPaths()
{
	for (int32 i = 0; i < objectCount; i++)
	{
		Chaos::Pair<int32, int32> newEndPosititon = Chaos::Pair(
			FMath::RandRange(0, graphPointsCountY - 1),
			FMath::RandRange(0, graphPointsCountX - 1));
		
		m_objectPaths.Add(FindPath(m_currentPositions[i], newEndPosititon));
	}
}

void AGraphPawn::MoveObjects(float DeltaTime)
{
	for (int i = 0; i < objectCount; i++)
	{
		FTransform transform;
		pInstancedStaticMeshComponent->GetInstanceTransform(i, transform);

		int32 targetIndex = m_objectPaths[i].Find(m_currentPositions[i]) + 1;
		if (targetIndex == -1 || targetIndex >= m_objectPaths[i].Num())
		{
			Chaos::Pair<int32, int32> newEndPosititon = Chaos::Pair(
			FMath::RandRange(0, graphPointsCountY - 1),
			FMath::RandRange(0, graphPointsCountX - 1));
			
			m_objectPaths[i] = FindPath(m_currentPositions[i], newEndPosititon);
			continue;
		}

		FVector currentLocation = transform.GetLocation();
		FVector targetLocation = m_nodes[m_objectPaths[i][targetIndex].First][m_objectPaths[i][targetIndex].Second];
		transform.SetLocation(
			currentLocation + (targetLocation - currentLocation).GetSafeNormal() * DeltaTime * objectVelocity);

		pInstancedStaticMeshComponent->UpdateInstanceTransform(i, transform, false, true, false);

		if (FMath::IsNearlyZero(FVector::Distance(currentLocation, targetLocation), 1))
		{
			m_currentPositions[i] = m_objectPaths[i][targetIndex];
		}
	}
}

TArray<Chaos::Pair<int32, int32>> AGraphPawn::FindPath(Chaos::Pair<int32, int32> StartPosition,
                                                       Chaos::Pair<int32, int32> EndPosition)
{
	TArray<std::tuple<TArray<int32>, float, float>> vertices;
	vertices.Init(std::tuple(TArray<int32>(), MAX_flt, MAX_flt), m_adjTable.Num());

	int32 currentPosition = StartPosition.First * graphPointsCountX + StartPosition.Second;
	const int32 targetPosition = EndPosition.First * graphPointsCountX + EndPosition.Second;

	vertices[currentPosition] = std::tuple<TArray<int32>, float, float>({currentPosition}, 0, 0);
	TArray<int32> priorityQueue = {currentPosition};

	while (priorityQueue.Num() != 0 && priorityQueue.Last() != targetPosition)
	{
		int32 i = priorityQueue.Pop();

		TArray<int32> newVertices;
		for (int j = 0; j < m_adjTable[i].Num(); j++)
		{
			if (m_adjTable[i][j] == 0 || std::get<0>(vertices[j]).Num() != 0)
				continue;

			TArray<int32> visited = std::get<0>(vertices[i]);
			visited.Push(j);

			float graphLength = std::get<1>(vertices[i]) + FVector::Distance(
				m_nodes[i / 10][i % 10], m_nodes[j / 10][j % 10]);
			float euclideLength = FVector::Distance(m_nodes[j / 10][j % 10],
			                                        m_nodes[EndPosition.First][EndPosition.Second]);

			vertices[j] = std::tuple(visited, graphLength, euclideLength);
			priorityQueue.Push(j);
		}

		priorityQueue.Sort([&vertices](int32 A, int32 B)
		{
			return std::get<1>(vertices[A]) + std::get<2>(vertices[A]) > std::get<1>(vertices[B]) + std::get<2>(
				vertices[B]);
		});
	}

	if (priorityQueue.Num() == 0)
		return TArray<Chaos::Pair<int32, int32>>();

	TArray<Chaos::Pair<int32, int32>> result;
	for (auto index : std::get<0>(vertices[priorityQueue.Last()]))
	{
		result.Add(Chaos::Pair(index / 10, index % 10));
	}

	return result;
}
