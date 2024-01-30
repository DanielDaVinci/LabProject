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

	pInstancedStaticMeshObjectsComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("ISMObjectsComponent");
	pInstancedStaticMeshObjectsComponent->SetupAttachment(GetRootComponent());
	pInstancedStaticMeshObjectsComponent->NumCustomDataFloats = 3;
	
	pInstancedStaticMeshNodesComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("ISMNodesComponent");
	pInstancedStaticMeshNodesComponent->SetupAttachment(GetRootComponent());
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

	playerController->OnMouseWheel.BindUObject(this, &AGraphPawn::OnMouseWheel);
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

void AGraphPawn::OnMouseWheel(float Amount)
{
	float newLength = FMath::Clamp(
		pSpringArmComponent->TargetArmLength - Amount * cameraSensitivity.mouseWheelSensitivity,
		0,
		FMath::Max(parameters.graphPointsCountX, parameters.graphPointsCountY) * (parameters.graphStep + 1));

	pSpringArmComponent->TargetArmLength = newLength;
}

void AGraphPawn::MouseInputRotate(float DeltaTime) const
{
	const FVector2D mouseDelta = m_currentMousePosition - m_startMousePosition;
	const FRotator deltaRotation = FRotator(-1 * mouseDelta.Y, mouseDelta.X,0.0f) * cameraSensitivity.mouseMoveSensitivity;
	
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
	exp.Init(FVector::ZeroVector, parameters.graphPointsCountX);
	m_nodes.Init(exp, parameters.graphPointsCountY);

	FVector centerOffset(
		-1 * parameters.graphPointsCountX * parameters.graphStep / 2.0f,
		-1 * parameters.graphPointsCountY * parameters.graphStep / 2.0f,
		0.0f);

	for (int32 i = 0; i < parameters.graphPointsCountY; i++)
	{
		for (int32 j = 0; j < parameters.graphPointsCountX; j++)
		{
			FVector offset = {
				FMath::FRandRange(-1 * parameters.graphStep / 2.0f, parameters.graphStep / 2.0f),
				FMath::FRandRange(-1 * parameters.graphStep / 2.0f, parameters.graphStep / 2.0f),
				FMath::FRandRange(-1 * parameters.graphStep / 2.0f, parameters.graphStep / 2.0f)
			};

			m_nodes[i][j] = centerOffset + FVector(j * parameters.graphStep, i * parameters.graphStep, 0.0f) + offset;

			pInstancedStaticMeshNodesComponent->AddInstance(FTransform(m_nodes[i][j]));
		}
	}
}

void AGraphPawn::InitAdjTable()
{
	int32 verticesAmount = parameters.graphPointsCountX * parameters.graphPointsCountY;
	
	TArray<int32> visitedVertices;
	visitedVertices.Init(0, verticesAmount);
	
	m_adjTable.Init(visitedVertices, verticesAmount);

	TArray<Chaos::Pair<int32, int32>> queueVertices = {{0, 0}};
	
	while (queueVertices.Num() != 0 && verticesAmount != 0)
	{
		Chaos::Pair<int32, int32> currentPosition = queueVertices.Last();
		
		int32 y = currentPosition.First * parameters.graphPointsCountX + currentPosition.Second;
		visitedVertices[y] = 1;

		TArray<int32> possiblePaths;
		
		if (currentPosition.Second - 1 >= 0)
			possiblePaths.Push(currentPosition.First * parameters.graphPointsCountX + currentPosition.Second - 1);
		if (currentPosition.Second + 1 < parameters.graphPointsCountX)
			possiblePaths.Push(currentPosition.First * parameters.graphPointsCountX + currentPosition.Second + 1);
		if (currentPosition.First - 1 >= 0)
			possiblePaths.Push((currentPosition.First - 1) * parameters.graphPointsCountX + currentPosition.Second);
		if (currentPosition.First + 1 < parameters.graphPointsCountY)
			possiblePaths.Push((currentPosition.First + 1) * parameters.graphPointsCountX + currentPosition.Second);
		
		possiblePaths = possiblePaths.FilterByPredicate([&visitedVertices](int32 value)
		{
			return visitedVertices[value] == 0;
		});

		if (possiblePaths.Num() == 0)
		{
			queueVertices.Pop();
			continue;
		}

		int32 x = possiblePaths[FMath::RandRange(0, possiblePaths.Num() - 1)];
		m_adjTable[y][x] = m_adjTable[x][y] = 1;

		queueVertices.Push({x / parameters.graphPointsCountX, x % parameters.graphPointsCountX});
		verticesAmount -= 1;
	}
	
	for (int32 i = 0; i < parameters.graphPointsCountY; i++)
	{
		for (int32 j = 0; j < parameters.graphPointsCountX; j++)
		{
			int32 y = i * parameters.graphPointsCountX + j;
	
			if (i + 1 < parameters.graphPointsCountY)
			{
				int32 x = (i + 1) * parameters.graphPointsCountX + j;
				
				if (m_adjTable[y][x] == 0)
				{
					m_adjTable[y][x] = m_adjTable[x][y] = FMath::RandRange(0, 1);
				}
			}
			if (j + 1 < parameters.graphPointsCountX)
			{
				int32 x = i * parameters.graphPointsCountX + j + 1;

				if (m_adjTable[y][x] == 0)
				{
					m_adjTable[y][x] = m_adjTable[x][y] = FMath::RandRange(0, 1);
				}
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
		pInstancedStaticMeshObjectsComponent->GetInstanceTransform(i, transform);

		locations.Add(transform.GetLocation());
	}

	return locations;
}

void AGraphPawn::InitObjects()
{
	for (int32 i = 0; i < objectCount; i++)
	{
		m_currentPositions.Add(Chaos::Pair(
			FMath::RandRange(0, parameters.graphPointsCountY - 1),
			FMath::RandRange(0, parameters.graphPointsCountX - 1)));

		FVector position = m_nodes[m_currentPositions[i].First][m_currentPositions[i].Second];

		pInstancedStaticMeshObjectsComponent->AddInstance(FTransform(position));
		pInstancedStaticMeshObjectsComponent->SetCustomDataValue(i, 0, objectColors[i % objectColors.Num()].R);
		pInstancedStaticMeshObjectsComponent->SetCustomDataValue(i, 1, objectColors[i % objectColors.Num()].G);
		pInstancedStaticMeshObjectsComponent->SetCustomDataValue(i, 2, objectColors[i % objectColors.Num()].B);
	}
}

void AGraphPawn::InitPaths()
{
	for (int32 i = 0; i < objectCount; i++)
	{
		Chaos::Pair<int32, int32> newEndPosititon = Chaos::Pair(
			FMath::RandRange(0, parameters.graphPointsCountY - 1),
			FMath::RandRange(0, parameters.graphPointsCountX - 1));
		
		m_objectPaths.Add(FindPath(m_currentPositions[i], newEndPosititon));
	}
}

void AGraphPawn::MoveObjects(float DeltaTime)
{
	for (int i = 0; i < objectCount; i++)
	{
		FTransform transform;
		pInstancedStaticMeshObjectsComponent->GetInstanceTransform(i, transform);

		int32 targetIndex = m_objectPaths[i].Find(m_currentPositions[i]) + 1;
		if (targetIndex == -1 || targetIndex >= m_objectPaths[i].Num())
		{
			Chaos::Pair<int32, int32> newEndPosititon = Chaos::Pair(
			FMath::RandRange(0, parameters.graphPointsCountY - 1),
			FMath::RandRange(0, parameters.graphPointsCountX - 1));
			
			m_objectPaths[i] = FindPath(m_currentPositions[i], newEndPosititon);
			continue;
		}

		FVector currentLocation = transform.GetLocation();
		FVector targetLocation = m_nodes[m_objectPaths[i][targetIndex].First][m_objectPaths[i][targetIndex].Second];
		
		FVector shift = (targetLocation - currentLocation).GetSafeNormal() * DeltaTime * objectVelocity;
		shift = shift.GetClampedToMaxSize((targetLocation - currentLocation).Size());
		
		transform.SetLocation(currentLocation + shift);
		pInstancedStaticMeshObjectsComponent->UpdateInstanceTransform(i, transform, false, true, false);

		if (FVector::PointsAreNear(transform.GetLocation(), targetLocation, 0.01f))
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

	int32 currentPosition = StartPosition.First * parameters.graphPointsCountX + StartPosition.Second;
	const int32 targetPosition = EndPosition.First * parameters.graphPointsCountX + EndPosition.Second;

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
				m_nodes[i / parameters.graphPointsCountX][i % parameters.graphPointsCountX],
				m_nodes[j / parameters.graphPointsCountX][j % parameters.graphPointsCountX]);
			
			float euclideLength = FVector::Distance(
				m_nodes[j / parameters.graphPointsCountX][j % parameters.graphPointsCountX],
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
		result.Add(Chaos::Pair(
			index / parameters.graphPointsCountX,
			index % parameters.graphPointsCountX));
	}

	return result;
}
