// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GraphPlayerController.generated.h"

DECLARE_DELEGATE_OneParam(FOnMouseDownSignature, FVector2D);
DECLARE_DELEGATE_OneParam(FOnMousePressedMoveSignature, FVector2D);
DECLARE_DELEGATE_OneParam(FOnMouseWheelSignature, float);

UCLASS()
class LABPROJECT_API AGraphPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGraphPlayerController();
	
	FOnMouseDownSignature OnMouseDown;
	FOnMousePressedMoveSignature OnMousePressedMove;
	FOnMouseWheelSignature OnMouseWheel;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MouseInput")
	float mouseHandleTimeRate = 0.1f;

	virtual void SetupInputComponent() override;

	void OnMouseLeftPressed();
	void OnMouseLeftReleased();

	void OnMouseWheelAxis(float Amount);
	
private:
	FTimerHandle m_mouseMoveTimer;

	UFUNCTION()
	void OnMouseMoveTimerUpdate();
};
