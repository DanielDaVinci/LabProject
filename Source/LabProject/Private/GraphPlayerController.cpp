// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphPlayerController.h"

AGraphPlayerController::AGraphPlayerController()
{
	bShowMouseCursor = true;
}

void AGraphPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &AGraphPlayerController::OnMouseLeftPressed);
	InputComponent->BindAction("LeftMouse", IE_Released, this, &AGraphPlayerController::OnMouseLeftReleased);

	InputComponent->BindAxis("WheelMouse", this, &AGraphPlayerController::OnMouseWheelAxis);
}

void AGraphPlayerController::OnMouseLeftPressed()
{
	FVector2D mousePosition;
	GetMousePosition(mousePosition.X, mousePosition.Y);

	OnMouseDown.ExecuteIfBound(mousePosition);

	if (!GetWorld())
		return;

	GetWorld()->GetTimerManager().SetTimer(m_mouseMoveTimer, this, &AGraphPlayerController::OnMouseMoveTimerUpdate,
	                                       mouseHandleTimeRate, true, 0.0f);
}

void AGraphPlayerController::OnMouseLeftReleased()
{
	if (!GetWorld())
		return;

	GetWorld()->GetTimerManager().ClearTimer(m_mouseMoveTimer);
}

void AGraphPlayerController::OnMouseWheelAxis(float Amount)
{
	OnMouseWheel.ExecuteIfBound(Amount);
}

void AGraphPlayerController::OnMouseMoveTimerUpdate()
{
	static FVector2D prevMousePosition = {};

	FVector2D mousePosition;
	GetMousePosition(mousePosition.X, mousePosition.Y);

	FVector2D deltaVector = mousePosition - prevMousePosition;

	if (!deltaVector.IsNearlyZero())
	{
		OnMousePressedMove.ExecuteIfBound(mousePosition);
	}
}
