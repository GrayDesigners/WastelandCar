// Fill out your copyright notice in the Description page of Project Settings.

#include "Player_Car.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

APlayer_Car::APlayer_Car() {
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked< UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	//Adjusting the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	//Torque setup
	Vehicle4W->MaxEngineRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 500.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	//Adjust the steering
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(40.0f, 0.7f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 1.6f);

	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	//Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	/*//Creating a springarm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = true;

	//Creating a camera
	Camera -> CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.0f;
	*/
}

void APlayer_Car::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateInAirControl(DeltaTime);
}
void APlayer_Car::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &APlayer_Car::ApplyThrottle);
	PlayerInputComponent->BindAxis(NAME_SteerInput, this, &APlayer_Car::ApplySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayer_Car::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &APlayer_Car::Turn);
	/*
		PlayerInputComponent->BindAction("Handbrake", If_Pressed, this, &APlayer_Car::OnHandbrakePressed);
		PlayerInputComponent->BindAction("Handbrake", If_Released, this, &APlayer_Car::OnHandbrakeReleased);
		*/
}

void APlayer_Car::ApplyThrottle(float Val) {
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void APlayer_Car::ApplySteering(float Val) {
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void APlayer_Car::LookUp(float Val) {
	if (Val != 0.f) {
		AddControllerPitchInput(Val);
	}
}

void APlayer_Car::Turn(float Val) {
	AddControllerYawInput(Val);
}

void APlayer_Car::OnHandbrakePressed() {
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void APlayer_Car::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void APlayer_Car::UpdateInAirControl(float DeltaTime) {
}