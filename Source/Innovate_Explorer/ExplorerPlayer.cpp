// Fill out your copyright notice in the Description page of Project Settings.

#include "ExplorerPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AExplorerPlayer::AExplorerPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    DroneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMesh"));
    DroneMesh->SetupAttachment(RootComponent);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bEnableCameraLag = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    FocusPoint = FVector(0.f, 0.f, 0.f);
    OrbitDistance = 500.f;
    OrbitSpeed = 90.f;
    MoveSpeed = 1000.f;
    bLeftMousePressed = false;
    bRightMousePressed = false;
    OrbitYaw = 0.f;
    OrbitPitch = 0.f;
}

void AExplorerPlayer::BeginPlay()
{
    Super::BeginPlay();
    
    SetActorLocation(FocusPoint + FVector(OrbitDistance, 0.f, 0.f));
    SetActorRotation((FocusPoint - GetActorLocation()).Rotation());
}

void AExplorerPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Interpolar os ângulos de órbita para suavidade
    OrbitYaw = FMath::FInterpTo(OrbitYaw, OrbitYawTarget, DeltaTime, 10.0f);
    OrbitPitch = FMath::FInterpTo(OrbitPitch, OrbitPitchTarget, DeltaTime, 10.0f);

    // Calcular a localização de órbita com base nos ângulos
    FVector OrbitLocation;
    float RadYaw = FMath::DegreesToRadians(OrbitYaw);
    float RadPitch = FMath::DegreesToRadians(OrbitPitch);
    OrbitLocation.X = FocusPoint.X + OrbitDistance * FMath::Cos(RadPitch) * FMath::Cos(RadYaw);
    OrbitLocation.Y = FocusPoint.Y + OrbitDistance * FMath::Cos(RadPitch) * FMath::Sin(RadYaw);
    OrbitLocation.Z = FocusPoint.Z + OrbitDistance * FMath::Sin(RadPitch);

    // Interpolar a posição atual para a localização alvo
    FVector CurrentLocation = GetActorLocation();
    FVector SmoothedLocation = FMath::VInterpTo(CurrentLocation, OrbitLocation, DeltaTime, 1);
    SetActorLocation(SmoothedLocation);

    // Ajustar a rotação para sempre olhar para o ponto focal
    SetActorRotation((FocusPoint - SmoothedLocation).Rotation());
}
void AExplorerPlayer::HandleMouseMove(const FInputActionValue& Value)
{
    FVector2D MouseDelta = Value.Get<FVector2D>();
    float DeltaTime = GetWorld()->GetDeltaSeconds();


    if (bRightMousePressed && !bLeftMousePressed)
    {
        HandleOrbitMovement(MouseDelta);
    }
}

void AExplorerPlayer::HandleOrbitMovement(FVector2D MouseDelta)
{
    float distance = FVector::Dist(GetActorLocation(), FocusPoint);
        
    float sensitivity = Sensitivity / distance;
    OrbitYawTarget += MouseDelta.X * OrbitSpeed * sensitivity;
    OrbitPitchTarget += MouseDelta.Y * OrbitSpeed * sensitivity * -1.f;
    OrbitPitchTarget = FMath::Clamp(OrbitPitchTarget, 20, 70.f); // Limitar pitch
}

void AExplorerPlayer::Zoom(const FInputActionValue& Value)
{
    float ZoomValue = Value.Get<float>();
    OrbitDistance = FMath::Clamp(OrbitDistance - ZoomValue * 50.f, 100.f, 2000.f);
}

void AExplorerPlayer::OnLeftMousePressed()
{
    bLeftMousePressed = true;
}

void AExplorerPlayer::OnLeftMouseReleased()
{
    bLeftMousePressed = false;
}

void AExplorerPlayer::OnRightMousePressed()
{
    bRightMousePressed = true;
}

void AExplorerPlayer::OnRightMouseReleased()
{
    bRightMousePressed = false;
}

void AExplorerPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DroneMappingContext, 0);
        }
    }

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Movimento do mouse
        EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AExplorerPlayer::HandleMouseMove);
        
        // Botão esquerdo
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &AExplorerPlayer::OnLeftMousePressed);
        EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Completed, this, &AExplorerPlayer::OnLeftMouseReleased);
        
        // Botão direito
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &AExplorerPlayer::OnRightMousePressed);
        EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Completed, this, &AExplorerPlayer::OnRightMouseReleased);
        
        // Zoom
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AExplorerPlayer::Zoom);
    }
}
