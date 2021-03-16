// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShooterCharacter.generated.h"

class AGun;
class ALauncher;

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	
	void Shoot();
	
	//Get Variables for BTTask_Shoot
	float GetMaxWeaponRange();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxAmmo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Ammo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxLauncherAmmo = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 LauncherAmmo;

	UPROPERTY(EditAnywhere)
	float LineofSightRadius =  500;

	UPROPERTY(BlueprintReadOnly)
	int32 ActiveWeaponIndex;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* EquipWeaponAnimation;
	
private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	void SwitchWeaponScrollUp();
	void SwitchWeaponScrollDown();
	void DisableWeapon();
	void EnableWeapon();

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ALauncher> LauncherClass;

	UPROPERTY(EditAnywhere)
	USoundBase* EmptyGunMuzzleSound;

	UPROPERTY(EditAnywhere)
	USoundBase* EquipSound;

	UPROPERTY()
	AGun* Gun;

	UPROPERTY()
	ALauncher* Launcher;
};
