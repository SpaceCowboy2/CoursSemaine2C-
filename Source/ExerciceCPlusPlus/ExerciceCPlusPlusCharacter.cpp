// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExerciceCPlusPlusCharacter.h"

#include "FloorBehaviour.h"
#include "ExerciceCPlusPlusGameMode.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// AExerciceCPlusPlusCharacter

AExerciceCPlusPlusCharacter::AExerciceCPlusPlusCharacter()
{

	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->AddRelativeLocation(FVector(250.0f,0,0));
	HoldingComponent->SetupAttachment(GetCapsuleComponent());

	CurrentItem = NULL;
	bCanMove = true;
	bInspecting = false;

	//PitchMax = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax;
	//PitchMin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AExerciceCPlusPlusCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (healthValue <= 0)
	{
		AExerciceCPlusPlusCharacter::CallRestartPlayer();
		healthValue = 100;
	}
	
	Start = GetCapsuleComponent()->GetComponentLocation();
	ForwardVector = GetCapsuleComponent()->GetForwardVector();
	End = ((ForwardVector * 200.0f) + Start);
	
	DrawDebugLine(GetWorld(),Start,End,FColor::Green,false,1,0,1);

	if(!bHoldingItem)
	{
		if(GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,DefaultComponentQueryParams,DefaultResponseParams))
		{
			if(Hit.GetActor()->GetClass()->IsChildOf(APickupAndRotateActor::StaticClass()))
			{
				CurrentItem = Cast<APickupAndRotateActor>(Hit.GetActor());
			}
		}
		else
		{
			CurrentItem = NULL;
		}
	}
	

	if(bInspecting)
	{
		if(bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation(FVector(0.0f,50.0f,50.0f));
			CurrentItem->RotateActor();
		}
		else
		{
			
		}
	}
	else
	{
		if(bHoldingItem)
		{
			HoldingComponent->SetRelativeLocation((FVector(150.0f,0,0)));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AExerciceCPlusPlusCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//Sets up an input key action to call Restart Player.
	PlayerInputComponent->BindAction("Restart", IE_Pressed, this, &AExerciceCPlusPlusCharacter::CallRestartPlayer);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AExerciceCPlusPlusCharacter::Fire);
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AExerciceCPlusPlusCharacter::OnAction);

	//PlayerInputComponent->BindAction("Inspect", IE_Pressed, this, &AExerciceCPlusPlusCharacter::OnInspect);
	//PlayerInputComponent->BindAction("Inspect", IE_Released, this, &AExerciceCPlusPlusCharacter::OnInspectReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AExerciceCPlusPlusCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AExerciceCPlusPlusCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AExerciceCPlusPlusCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AExerciceCPlusPlusCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AExerciceCPlusPlusCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AExerciceCPlusPlusCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AExerciceCPlusPlusCharacter::OnResetVR);
}


void AExerciceCPlusPlusCharacter::OnResetVR()
{
	// If ExerciceCPlusPlus is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in ExerciceCPlusPlus.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AExerciceCPlusPlusCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AExerciceCPlusPlusCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AExerciceCPlusPlusCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AExerciceCPlusPlusCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AExerciceCPlusPlusCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f && bCanMove))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AExerciceCPlusPlusCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f && bCanMove))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AExerciceCPlusPlusCharacter::Destroyed()
{
	Super::Destroyed();

	// Example to bind to OnPlayerDied event in GameMode. 
	if (UWorld* World = GetWorld())
	{
		if (AExerciceCPlusPlusGameMode* GameMode = Cast<AExerciceCPlusPlusGameMode>(World->GetAuthGameMode()))
		{
			GameMode->GetOnPlayerDied().Broadcast(this);
		}
	}
}

void AExerciceCPlusPlusCharacter::CallRestartPlayer()
{
	//Get a reference to the Pawn Controller.
	AController* CortollerRef = GetController();

	//Destroy the Player.   
	Destroy();
	
	//Get the World and GameMode in the world to invoke its restart player function.
	if (UWorld* World = GetWorld())
	{
		if (AExerciceCPlusPlusGameMode* GameMode = Cast<AExerciceCPlusPlusGameMode>(World->GetAuthGameMode()))
		{
			GameMode->RestartPlayer(CortollerRef);
		}
	}
}

void AExerciceCPlusPlusCharacter::OnAction()
{
	if(CurrentItem && !bInspecting)
	{
		ToggleItemPickUp();
	}
}

void AExerciceCPlusPlusCharacter::OnInspect()
{
	if(bHoldingItem)
	{
		LastRotation = GetControlRotation();
		ToggleMovement();
	}
	else
	{
		bInspecting = true;
	}
}

void AExerciceCPlusPlusCharacter::OnInspectReleased()
{
	if(bInspecting && bHoldingItem)
	{
		GetController()->SetControlRotation(LastRotation);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
		ToggleMovement();
	}
	else
	{
		bInspecting = false;
	}
}

void AExerciceCPlusPlusCharacter::ToggleMovement()
{
	bCanMove = !bCanMove;
	bInspecting = !bInspecting;
	bUseControllerRotationYaw = ! bUseControllerRotationYaw;
}

void AExerciceCPlusPlusCharacter::ToggleItemPickUp()
{
	if(CurrentItem)
	{
		bHoldingItem = !bHoldingItem;
		CurrentItem->Pickup();

		if(!bHoldingItem)
		{
			CurrentItem = NULL;
		}
	}
}

void AExerciceCPlusPlusCharacter::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = GetCapsuleComponent()->GetForwardVector() + FTransform(GetCapsuleComponent()->GetComponentRotation()).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = GetCapsuleComponent()->GetComponentRotation();
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
				
			}
		}
	}
}

