// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Car.generated.h"

/**
 *
 */
UCLASS()
class WASTELANDCAR_API ACar : public AWheeledVehicle
{
	GENERATED_BODY()

public:
	ACar();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/*Throttle/sterring*/
	void ApplyThrottle(float Val);
	void ApplySteering(float Val);

	/*Look around*/
	void LookUp(float Val);
	void Turn(float Val);

	/*handbrake*/
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	/*In air physics*/
	void UpdateInAirControl(float DeltaTime);

	/*Spring arm that will offset the camera*/
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* SpringArm;

	/*Camera that will be our viewpoint*/
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* Camera;
};
