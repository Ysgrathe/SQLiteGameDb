// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TutorialDemoCharacter.generated.h"

UCLASS(config=Game)
class ATutorialDemoCharacter : public ACharacter
{
public:
	ATutorialDemoCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void BeginPlay() override;
	
protected:

	void MoveForward(float Value);

	void MoveRight(float Value);
	
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	GENERATED_BODY()
};
