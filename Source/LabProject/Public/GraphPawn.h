// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GraphPawn.generated.h"

class AGraphNode;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LABPROJECT_API AGraphPawn : public APawn
{
	GENERATED_BODY()

public:
	AGraphPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Scene Component")
	USceneComponent* pSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Spring ArmComponent")
	USpringArmComponent* pSpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="CameraComponent")
	UCameraComponent* pCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Instanced Static Mesh Component")
	UInstancedStaticMeshComponent* pInstancedStaticMeshComponent;
	
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera")
	float cameraSensitivity = 1.0f;
	
	void InputBind();

	void OnMouseDown(FVector2D MousePosition);
	void OnMouseMove(FVector2D MousePosition);

	void MouseInputRotate(float DeltaTime) const;
	
private:
	FVector2D m_startMousePosition;
	FVector2D m_currentMousePosition;
	FRotator m_startCameraRotation;

public:
	TArray<TArray<FVector>>& GetNodes() { return m_nodes; }
	TArray<TArray<int32>>& GetAdjTable() { return m_adjTable; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 graphPointsCountX = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 graphPointsCountY = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float graphStep = 10.0f;

	void InitGraph();
	
	void InitNodes();
	void InitAdjTable();

private:
	TArray<TArray<FVector>> m_nodes;
	TArray<TArray<int32>> m_adjTable;

public:
	TArray<FColor>& GetObjectColors() { return objectColors; }
	
	TArray<Chaos::Pair<int32, int32>>& GetCurrentPositions() { return m_currentPositions; }
	TArray<TArray<Chaos::Pair<int32, int32>>>& GetPaths() { return m_objectPaths; }

	TArray<FVector> GetCurrentLocations() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	int32 objectCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	TArray<FColor> objectColors = {FColor::Red, FColor::Green, FColor::Blue};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	float objectVelocity = 1.0f;

	void InitObjects();
	void InitPaths();

	void MoveObjects(float DeltaTime);
	
private:
	TArray<Chaos::Pair<int32, int32>> m_currentPositions;
	TArray<TArray<Chaos::Pair<int32, int32>>> m_objectPaths;

	TArray<Chaos::Pair<int32, int32>> FindPath(Chaos::Pair<int32, int32> StartPosition, Chaos::Pair<int32, int32> EndPosition);
	
};
