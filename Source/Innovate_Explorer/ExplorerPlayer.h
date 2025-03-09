// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "ExplorerPlayer.generated.h"

UCLASS()
class INNOVATE_EXPLORER_API AExplorerPlayer : public APawn
{
    GENERATED_BODY()

public:
    AExplorerPlayer();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Componentes
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* DroneMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* Camera;

    // Input Actions
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseMoveAction; // Captura movimento do mouse

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LeftMouseAction; // Botão esquerdo

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* RightMouseAction; // Botão direito

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ZoomAction; // Scroll do mouse

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DroneMappingContext;

    // Propriedades do Drone
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
    FVector FocusPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
    float OrbitDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
    float OrbitSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
    float MoveSpeed;

    UPROPERTY(EditAnywhere, Category = "Input")
    float OrbitSmoothFactor = 1.f;

    UPROPERTY(EditAnywhere, Category = "Input")
    float Sensitivity = 1.f;

    float OrbitYawTarget;
    float OrbitPitchTarget;

    UPROPERTY(VisibleAnywhere)
    FVector TargetLocation;

private:
    // Funções de movimento
    void HandleMouseMove(const FInputActionValue& Value);
    void HandleOrbitMovement(FVector2D MouseDelta);
    void Zoom(const FInputActionValue& Value);
    void OnLeftMousePressed();
    void OnLeftMouseReleased();
    void OnRightMousePressed();
    void OnRightMouseReleased();

    // Estados dos botões
    bool bLeftMousePressed;
    bool bRightMousePressed;

    // Ângulos de órbita
    float OrbitYaw;   // Rotação horizontal
    float OrbitPitch; // Rotação vertical
};