// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Launcher.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Ammo = MaxAmmo;
	LauncherAmmo = MaxLauncherAmmo;
	
	ActiveWeaponIndex = 0;


	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);

	Launcher = GetWorld()->SpawnActor<ALauncher>(LauncherClass);
	Launcher->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Launcher->SetOwner(this);
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ActiveWeaponIndex == 0)
	{
		//Enable Gun and Disable Launcher
		EnableWeapon();
		DisableWeapon();
	}
	else if (ActiveWeaponIndex == 1)
	{
		//Enable Launcher and Disable Gun
		EnableWeapon();
		DisableWeapon();
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("ScrollUp"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SwitchWeaponScrollUp);
	PlayerInputComponent->BindAction(TEXT("ScrollDown"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SwitchWeaponScrollDown);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	if (IsDead())
	{
		ASimpleShooterGameModeBase *GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

void AShooterCharacter::MoveForward(float AxisValue) 
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue) 
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue) 
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue) 
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::SwitchWeaponScrollUp() 
{
	if(ActiveWeaponIndex == 0)
	{
		return;
	}

	//Switch to Rifle
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquipSound, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	ActiveWeaponIndex = 0;
}

void AShooterCharacter::SwitchWeaponScrollDown() 
{
	if(ActiveWeaponIndex == 1)
	{
		return;
	}

	//Switch to Launcher
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquipSound, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());
	ActiveWeaponIndex = 1;
}

void AShooterCharacter::DisableWeapon() 
{
	if(ActiveWeaponIndex == 1)
	{
		Gun->SetActorHiddenInGame(true);
		Gun->SetActorEnableCollision(false);
		Gun->SetActorTickEnabled(false);
	}
	else if (ActiveWeaponIndex == 0)
	{
		Launcher->SetActorHiddenInGame(true);
		Launcher->SetActorEnableCollision(false);
		Launcher->SetActorTickEnabled(false);
	}	
}

void AShooterCharacter::EnableWeapon() 
{
	if(ActiveWeaponIndex == 0)
	{
		Gun->SetActorHiddenInGame(false);
		Gun->SetActorEnableCollision(true);
		Gun->SetActorTickEnabled(true);
	}
	else if (ActiveWeaponIndex == 1)
	{
		Launcher->SetActorHiddenInGame(false);
		Launcher->SetActorEnableCollision(true);
		Launcher->SetActorTickEnabled(true);
	}	
}

void AShooterCharacter::Shoot() 
{
	if(ActiveWeaponIndex ==0)
	{
		if(Ammo <= 0)
		{
			UGameplayStatics::SpawnSoundAttached(EmptyGunMuzzleSound, Gun->Mesh, TEXT("MuzzleFlashSocket"));
		}
		else
		{
			--Ammo;
			Gun->PullTrigger();
		}
	}
	else if (ActiveWeaponIndex == 1)
	{
		if(LauncherAmmo <= 0)
		{
			UGameplayStatics::SpawnSoundAttached(EmptyGunMuzzleSound, Gun->Mesh, TEXT("MuzzleFlashSocket"));
		}
		else
		{
			--LauncherAmmo;
			Launcher->PullTrigger();
		}
	}	
}

float AShooterCharacter::GetMaxWeaponRange() 
{
	return Gun->MaxRange;
}

