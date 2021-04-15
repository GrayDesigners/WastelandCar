// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Player_Car.generated.h"

/**
 *
 */
UCLASS()
class WASTELANDCAR_API APlayer_Car : public AWheeledVehicle {
	GENERATED_BODY()

public:
	APlayer_Car();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//Throttle/Steering
	void ApplyThrottle(float Val);
	void ApplySteering(float Val);

	//Look around
	void LookUp(float Val);
	void Turn(float Val);

	//Handbrake
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	//Update in air physics
	void UpdateInAirControl(float DeltaTime);

protected:
	//Spring arm
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"))
		class USpringArmComponent* SpringArm;

	//Camera
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAcces = "true"))
		class USpringArmComponent* Camera;
};
