// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphNode.h"

#include "Components/InstancedStaticMeshComponent.h"

AGraphNode::AGraphNode()
{
	PrimaryActorTick.bCanEverTick = false;

	pSceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(pSceneComponent);

	pInstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedStaticMeshComponent");
	pInstancedStaticMeshComponent->SetupAttachment(GetRootComponent());
	pInstancedStaticMeshComponent->NumCustomDataFloats = 1;
}

void AGraphNode::BeginPlay()
{
	Super::BeginPlay();
	
}


