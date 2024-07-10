// Copyright Epic Games, Inc. All Rights Reserved.

#include "TerraCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimeActor.h"

ATerraCharacter::ATerraCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ProgressionComponent = CreateDefaultSubobject<UProgressionComponent>(FName("Progression Component"));
	ProgressionComponent->RegisterComponent();
	ProgressionComponent->Character = this;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(FName("Inventory Component"));
	InventoryComponent->RegisterComponent();
	InventoryComponent->Character = this;

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(FName("Status Component"));
	StatusComponent->RegisterComponent();
	StatusComponent->Character = this;

	

	
}

void ATerraCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	FString LocalString;
	LocalString = "Hunger";
	LocalString.Append("DecayBonusValue");

	if(!GetController()->IsPlayerController())
	{
		TArray<FSItem> LocalItems;
		InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
		FSItem LocalItem;
		LocalItem = InventoryComponent->FindItemByID("grain", LocalItems);

		if (LocalItem != InventoryComponent->NoneItem)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Cyan, FString::FromInt(*InventoryComponent->Inventory.Find(LocalItem)));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Cyan, "0");
		}

		InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
		LocalItem = InventoryComponent->FindItemByID("meat", LocalItems);

		if (LocalItem != InventoryComponent->NoneItem)
		{
			GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Orange, FString::FromInt(*InventoryComponent->Inventory.Find(LocalItem)));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Orange, "0");
		}

		float LocalWeight = *Modifiers.Find("Weight");

		GEngine->AddOnScreenDebugMessage(91, 1.0f, FColor::Cyan, FString::SanitizeFloat(LocalWeight));
	}
} 

void ATerraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Init();

	InventoryComponent->CreateItem(FName("meat"), 1);
	InventoryComponent->CreateItem(FName("grain"), 4);
	InventoryComponent->CreateItem(FName("grain"), 15);
	InventoryComponent->CreateItem(FName("hammerofstrength"), 1);
	InventoryComponent->CreateItem(FName("hammerofagility"), 1);
	InventoryComponent->CreateItem(FName("helmet"), 1);
	InventoryComponent->CreateItem(FName("helmet"), 1);

	ProgressionComponent->AddXPToAttribute("gathering", 200);

	InventoryComponent->RecalculateInventoryModifierBonuses();

	
}

void ATerraCharacter::InitProgressionComponent()
{
	ProgressionComponent->DT_Attributes = DT_Attributes;
	ProgressionComponent->DT_Skills = DT_Skills;
	ProgressionComponent->Init();
}

void ATerraCharacter::InitInventoryComponent()
{
	InventoryComponent->DT_Items = DT_Items;
	InventoryComponent->Character = this;
	InventoryComponent->Init();
	Modifiers.Add("Weight", 0.0f);
}

void ATerraCharacter::InitStatusComponent()
{
	StatusComponent->Init();
}

void ATerraCharacter::UpdateCharacterMovement()
{
	float NewMovementSpeed;
	NewMovementSpeed = *Modifiers.Find("MovementSpeedBase");
	NewMovementSpeed *= *Modifiers.Find("MovementSpeedModifier");
	NewMovementSpeed *= *Modifiers.Find("MovementSpeedCargoModifier");  

	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

void ATerraCharacter::Init()
{
	Modifiers.Add("MovementSpeedModifier", 1.0f);
	Modifiers.Add("MovementSpeedCargoModifier", 1.0f);

	UpdateCharacterMovement();
	InitStatusComponent();
	InitProgressionComponent();
	InitInventoryComponent();

	InventoryComponent->RecalculateInventoryModifierBonuses();

	TArray<AActor*> TimeActorArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimeActor::StaticClass(), TimeActorArray);
	TimeActor = Cast<ATimeActor>(TimeActorArray[0]); 

	TimeActor->AddCharacterToArray(this);
	
	
	if (GetController())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GEngine->AddOnScreenDebugMessage(10378, 1.0f, FColor::Cyan, "Ave");
	}
}



AActor* ATerraCharacter::FindClosetstActorFromArray(TArray<AActor*> ActorArray)
{
	AActor* OutputActor;
	OutputActor = this;

	float DistanceFromActors = -1.0f;

	for (auto& LocalActor : ActorArray)
	{
		if (DistanceFromActors < UKismetMathLibrary::Vector_Distance(LocalActor->GetActorLocation(), this->GetActorLocation()))
		{
			DistanceFromActors = UKismetMathLibrary::Vector_Distance(LocalActor->GetActorLocation(), this->GetActorLocation());
			OutputActor = LocalActor;
		}
	}
	if (OutputActor->IsValidLowLevel())
	{
		return OutputActor;
	}
	else
	{
		return 0;
	}
}