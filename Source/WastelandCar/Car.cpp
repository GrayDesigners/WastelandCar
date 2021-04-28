// Fill out your copyright notice in the Description page of Project Settings.

#include "Car.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

ACar::ACar() {
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked < UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	//Adjusting the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	//Torque setup
	Vehicle4W->MaxEngineRPM = 5700.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
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

	//Creating a springarm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.0f;
	SpringArm->bUsePawnControlRotation = true;

	//Creating a camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.0f;

	//Creating A WeaponBase
	WeaponBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBase"));
	WeaponBase->SetupAttachment(RootComponent);
	WeaponBase->SetMobility(EComponentMobility::Movable);

	//Creating a Muzzle Component
	Muzzle = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponBase);
}

void ACar::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateInAirControl(DeltaTime);
}

void ACar::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &ACar::ApplyThrottle);
	PlayerInputComponent->BindAxis(NAME_SteerInput, this, &ACar::ApplySteering);
	PlayerInputComponent->BindAxis("LookUp", this, &ACar::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ACar::Turn);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ACar::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ACar::OnHandbrakeReleased);
}

void ACar::ApplyThrottle(float Val) {
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void ACar::ApplySteering(float Val) {
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ACar::LookUp(float Val) {
	if (Val != 0.f) {
		AddControllerPitchInput(Val);
	}
}

void ACar::Turn(float Val) {
	;
	if (Val != 0.f) {
		AddControllerYawInput(Val);
	}
}

void ACar::OnHandbrakePressed() {
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ACar::OnHandbrakeReleased() {
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ACar::UpdateInAirControl(float DeltaTime) {
	if (UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement())) {
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		const FVector TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		const FVector TraceEnd = GetActorLocation() + FVector(0.f, 0.f, 200.f);

		FHitResult Hit;

		//check if car is flipped on its side, and check if the car is in air

		const bool bNotGrounded = FVector::DotProduct(GetActorUpVector(), FVector::UpVector) < 0.3f;

		if (bNotGrounded) {
			const float ForwardInput = InputComponent->GetAxisValue(NAME_ThrottleInput);
			const float RightInput = InputComponent->GetAxisValue(NAME_SteerInput);
			//In car is grounded allow player to roll the car over
			const float AirMovementForcePitch = 3.f;
			const float AirMovementForceRoll = bNotGrounded ? 20.f : 3.f;

			if (UPrimitiveComponent* VehicleMesh = Vehicle4W->UpdatedPrimitive) {
				const FVector MovementVector = FVector(RightInput * -AirMovementForceRoll, ForwardInput * AirMovementForcePitch, 0.f)*DeltaTime*25.f;
				const FVector NewAngularMovement = GetActorRotation().RotateVector(MovementVector);

				VehicleMesh->SetPhysicsAngularVelocity(NewAngularMovement, true);
			}
		}
	}
}