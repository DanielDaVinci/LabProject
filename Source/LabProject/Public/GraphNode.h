// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GraphNode.generated.h"

UCLASS()
class LABPROJECT_API AGraphNode : public AActor
{
	GENERATED_BODY()
	
public:	
	AGraphNode();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Scene Component")
	USceneComponent* pSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Instanced Static Mesh Component")
	UInstancedStaticMeshComponent* pInstancedStaticMeshComponent;
	
	virtual void BeginPlay() override;
	
};
