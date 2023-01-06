// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleSystemsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/Interaction.h"

//////////////////////////////////////////////////////////////////////////
// ASimpleSystemsCharacter

ASimpleSystemsCharacter::ASimpleSystemsCharacter()
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
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Collision"));
	InteractionCollision->SetupAttachment(RootComponent);
	InteractionCollision->SetGenerateOverlapEvents(true);

	InteractionComponent = CreateDefaultSubobject<UWidgetComponent>("Interaction Widget");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

#if WITH_EDITOR
	if (InteractionWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Interaction widget class not set."));
		return;
	}
#endif

	InteractionComponent->SetWidgetClass(InteractionWidgetClass);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASimpleSystemsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASimpleSystemsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASimpleSystemsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASimpleSystemsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASimpleSystemsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASimpleSystemsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASimpleSystemsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASimpleSystemsCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Inventory", IE_Pressed, InventoryComponent, &UInventoryComponent::ToggleInventory);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASimpleSystemsCharacter::Interact);
}

void ASimpleSystemsCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractionCollision->OnComponentBeginOverlap.AddDynamic(this, &ASimpleSystemsCharacter::InteractionOverlapBegin);
	InteractionCollision->OnComponentEndOverlap.AddDynamic(this, &ASimpleSystemsCharacter::InteractionOverlapEnd);
	InteractionComponent->SetVisibility(false);
}

#pragma region Interaction

void ASimpleSystemsCharacter::InteractionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IInteraction>(OtherActor))
	{
		if (InteractionAttachedToRef == nullptr)
		{
			AttachInteractionComponent(OtherComp);
		}
		//Remember all who we interact with.
		InteractingActors.AddUnique(OtherActor);
	}
}

void ASimpleSystemsCharacter::InteractionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<IInteraction>(OtherActor))
	{
		if (InteractionAttachedToRef == OtherActor)
		{
			AttachInteractionComponent(nullptr);
		}
		//Remember all who we interact with.
		InteractingActors.Remove(OtherActor);
	}
}

void ASimpleSystemsCharacter::AttachInteractionComponent(UPrimitiveComponent* OtherComp)
{
	if (OtherComp != nullptr)
	{
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

		InteractionComponent->AttachToComponent(OtherComp, AttachmentRules);
		InteractionComponent->SetVisibility(true);
		InteractionAttachedToRef = OtherComp->GetOwner();
	}
	else
	{
		InteractionComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		InteractionComponent->SetVisibility(false);
		InteractionAttachedToRef = nullptr;
	}
}

void ASimpleSystemsCharacter::Interact()
{
	if (InteractingActors.Num() > 0)
	{
		IInteraction* InteractionTarget = Cast<IInteraction>(InteractingActors[0]);
		//In case the target is destroyed after interaction, detach the interaction component from it before.
		if (InteractionTarget->ActorDestroyedOnInteract())
		{
			AttachInteractionComponent(nullptr);
		}
		Cast<IInteraction>(InteractingActors[0])->Interact();
	}
}

#pragma endregion Interaction Handling

#pragma region Standard Character Handling

void ASimpleSystemsCharacter::OnResetVR()
{
	// If SimpleSystems is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in SimpleSystems.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASimpleSystemsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASimpleSystemsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASimpleSystemsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASimpleSystemsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASimpleSystemsCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASimpleSystemsCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
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

#pragma endregion