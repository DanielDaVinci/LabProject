// Fill out your copyright notice in the Description page of Project Settings.


#include "LabProjectGameMode.h"

#include "GraphPawn.h"
#include "GraphPlayerController.h"

ALabProjectGameMode::ALabProjectGameMode()
{
	DefaultPawnClass = AGraphPawn::StaticClass();
	PlayerControllerClass = AGraphPlayerController::StaticClass();
}
