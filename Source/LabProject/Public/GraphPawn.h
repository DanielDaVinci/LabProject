// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CoreTypes.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Instanced Static Mesh Ojects Component")
	UInstancedStaticMeshComponent* pInstancedStaticMeshObjectsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Instanced Static Mesh Nodes Component")
	UInstancedStaticMeshComponent* pInstancedStaticMeshNodesComponent;
	
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Camera")
	FCameraSensitivity cameraSensitivity;
	
	void InputBind();

	void OnMouseDown(FVector2D MousePosition);
	void OnMouseMove(FVector2D MousePosition);
	void OnMouseWheel(float Amount);

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
	FGraphParameters parameters;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph", meta=(ClampMin="1"))
	int32 objectCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph")
	TArray<FColor> objectColors = {FColor::Red, FColor::Green, FColor::Blue};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Graph", meta=(ClampMin="1.0"))
	float objectVelocity = 1.0f;

	void InitObjects();
	void InitPaths();

	void MoveObjects(float DeltaTime);
	
private:
	TArray<Chaos::Pair<int32, int32>> m_currentPositions;
	TArray<TArray<Chaos::Pair<int32, int32>>> m_objectPaths;

	TArray<Chaos::Pair<int32, int32>> FindPath(Chaos::Pair<int32, int32> StartPosition, Chaos::Pair<int32, int32> EndPosition);
	
};
