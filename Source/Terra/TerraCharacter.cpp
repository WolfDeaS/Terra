// Copyright Epic Games, Inc. All Rights Reserved.

// TODO 
// I dont know why, but in CalculatePathDuration func, GetPathLength doesnt work properly, sometimes have shortcuts and another.
// Good thing that it always add distance. That not ideal, but better than remove it
// Problem in that: 500 points -> takes one step, nothing but distance changed -> 1000 point
// Also this feature has a very conditional character speed. Then I'll finish it
// Cursed function :D

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
#include "InteractionMark.h"
#include "InteractableActor.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationSystem.h"
#include "SocialCell.h"
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
			GEngine->AddOnScreenDebugMessage(92, 1.0f, FColor::Cyan, FString::FromInt(*InventoryComponent->Inventory.Find(LocalItem)));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(92, 1.0f, FColor::Cyan, "0");
		}

		InventoryComponent->Inventory.GenerateKeyArray(LocalItems);
		LocalItem = InventoryComponent->FindItemByID("meat", LocalItems);

		if (LocalItem != InventoryComponent->NoneItem)
		{
			GEngine->AddOnScreenDebugMessage(93, 1.0f, FColor::Orange, FString::FromInt(*InventoryComponent->Inventory.Find(LocalItem)));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(93, 1.0f, FColor::Orange, "0");
		}

		float LocalWeight = *Modifiers.Find("Weight");

		GEngine->AddOnScreenDebugMessage(91, 1.0f, FColor::Purple, FString::FromInt(bIsInteracted));
	}

	
	if (GetController()->IsPlayerController())
	{
		TArray<FSItem> LocalItem;
		InventoryComponent->Inventory.GenerateKeyArray(LocalItem);
		UE_LOG(LogTemp, Warning, TEXT("Tick: Modifier Bonuses Num %d"), LocalItem[0].ModifierBonuses.Num());
		UE_LOG(LogTemp, Warning, TEXT("Tick: CapacityModifier Num %d"), LocalItem[0].CapacityModifier.Num());
	}
} 

void ATerraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Init();

	FSItem Local = InventoryComponent->CreateItem(FName("barrel"), 0);
	
	Local.Durability--;
	InventoryComponent->AddItemToInventory(Local, 1);
	InventoryComponent->AddBeverage(InventoryComponent->AddBeverage(Local, "water", 25.0f), "wine", 5.0f);

	InventoryComponent->CreateItem(FName("grain"), 15);
	InventoryComponent->CreateItem(FName("meat"), 15);

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
	InventoryComponent->DT_Beverage = DT_Beverage;
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

	GetCharacterMovement()->MaxAcceleration = *Modifiers.Find("MovementAccelerationBase");
	
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
	}
}

AActor* ATerraCharacter::FindClosetstActorFromArray(TArray<AActor*> ActorArray, AActor* TargetActor)
{
	AActor* OutputActor;
	OutputActor = nullptr;

	float DistanceFromActors = -1.0f;

	for (auto& LocalActor : ActorArray)
	{
		if (DistanceFromActors < UKismetMathLibrary::Vector_Distance(LocalActor->GetActorLocation(), TargetActor->GetActorLocation()))
		{
			DistanceFromActors = UKismetMathLibrary::Vector_Distance(LocalActor->GetActorLocation(), TargetActor->GetActorLocation());
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

void ATerraCharacter::TargetActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor && Actor->IsA<AInteractableActor>())
	{
		AInteractableActor* LocalIA;
		LocalIA = Cast<AInteractableActor>(Actor);


		// Add InteractableMark through InteractableActor to MarksActor
		// TODO, It must recalc all MarkActor if Gathering or ther shit where this type of mark used for
		if (LocalIA->InteractionComponent->ReferencedActor && LocalIA->InteractionComponent->ReferencedActor->IsA<AInteractionMark>())
		{
			AInteractionMark* LocalIM;
			LocalIM = Cast<AInteractionMark>(LocalIA->InteractionComponent->ReferencedActor);

			FString LocalString;
			LocalString = UEnum::GetValueAsString(LocalIA->InteractionComponent->InteractID);
			LocalString.Append(";");
			LocalString.Append(LocalIA->InteractionComponent->AdditionalInfo.ToString());

			TArray<AInteractionMark*> LocalIMArray;
			if (MarksActor.Contains(*LocalString))
			{
				LocalIMArray = *MarksActor.Find(*LocalString);
			}
			
			if (!LocalIMArray.Contains(LocalIA->InteractionComponent->ReferencedActor))
			{
				LocalIMArray.Add(LocalIM);
				MarksActor.Add(*LocalString, LocalIMArray);

				return;
			}
		}

		UE_LOG(LogTemp , Error, TEXT("Perception Updated: Activity: %s"), *Activity.ToString());

		if (Activity != "None")
		{
			FSActivitiesCreation* ActivitiesDataFromDT;
			ActivitiesDataFromDT = SocialCell->DT_Acitivites->FindRow<FSActivitiesCreation>(Activity, TEXT("none"), false);

			bool CanIARestoreStatus = false;

			// ONLY FOR THE HERBS
			// Dont make this shit when we now that we need / not need it
			if (Activity != "Gathering")
			{
				UE_LOG(LogTemp, Warning, TEXT("Perception Updated: Activity Not Gathering"));

				// Only for the Statuses
				for (auto& RequestPair : RequestMap)
				{

					TArray<FName> Statuses = StatusComponent->DT_Statuses->GetRowNames();

					UE_LOG(LogTemp, Warning, TEXT("Perception Updated: Request - %s"), *RequestPair.Key.ToString());

					if (Statuses.Contains(RequestPair.Key))
					{
						UE_LOG(LogTemp, Warning, TEXT("Perception Updated: Status - %s"), *RequestPair.Key.ToString());

						AInteractionMark* LocalIM;
						LocalIM = Cast<AInteractionMark>(LocalIA->InteractionComponent->ReferencedActor);

						CanIARestoreStatus = StatusComponent->CanMarkRestoreStatus(LocalIM, RequestPair.Key);
						
						int NumofApproaches = 0;
						if (SocialCell->GetEfficencyMarkValue(LocalIM, 1, RequestPair.Key) > 0.0f)
						{
							NumofApproaches = FMath::CeilToInt(RequestPair.Value / SocialCell->GetEfficencyMarkValue(LocalIM, 1, RequestPair.Key));

							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("IntStorage_1", UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsInt("ActivityInt"));
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("IntStorage_2", UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsInt("ActivityIntAdditional"));

							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsName("LocalName", RequestPair.Key);
							SocialCell->ChangeCharacterWorkStatus(0, 1, this, LocalIM, SocialCell->FindActivityByMark(LocalIM->MarkType), Activity);
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("ActivityInt", 0);
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("ActivityIntAdditional", NumofApproaches);

							UE_LOG(LogTemp, Warning, TEXT("Perception Updated: For all SC, Activity - %s"), *SocialCell->FindActivityByMark(LocalIM->MarkType).ToString());

							break;
						}

						if (SocialCell->GetEfficencyMarkValue(LocalIM, 0, RequestPair.Key) > 0.0f)
						{
							NumofApproaches = FMath::CeilToInt(RequestPair.Value / SocialCell->GetEfficencyMarkValue(LocalIM, 0, RequestPair.Key));

							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("IntStorage_1", UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsInt("ActivityInt"));
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("IntStorage_2", UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsInt("ActivityIntAdditional"));

							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsName("LocalName", RequestPair.Key);
							SocialCell->ChangeCharacterWorkStatus(0, 1, this, LocalIM, SocialCell->FindActivityByMark(LocalIM->MarkType), Activity);
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("ActivityInt", 0);
							UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("ActivityIntAdditional", NumofApproaches);

							UE_LOG(LogTemp, Warning, TEXT("Perception Updated: For only Character, Activity - %s"), *SocialCell->FindActivityByMark(LocalIM->MarkType).ToString());

							break;
						}


						break;
					}
				}
			}
			
			

			// If this IA is part of Activity going to Mark. Character will change ActivityStatus and ActivityObject of BT become a LocalIA
			if (!ActivitiesDataFromDT->MarkForActivities.IsEmpty() || CanIARestoreStatus)
			{

				if (UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsObject("ActivityObject")->IsA<AInteractionMark>())
				{
					AInteractionMark* LocalIM;
					LocalIM = Cast<AInteractionMark>(UAIBlueprintHelperLibrary::GetBlackboard(this)->GetValueAsObject("ActivityObject"));

					if (LocalIM->ThisMarkActors.Contains(LocalIA))
					{
						UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsObject("ActivityObject", LocalIA);
						UAIBlueprintHelperLibrary::GetBlackboard(this)->SetValueAsInt("ActivityInt", 1);
					}
				}
			}
		}
	}
}

float ATerraCharacter::CalculatePathDuration(const FVector& PathStart, const FVector& PathEnd) // Return duration in seconds, basen on length / speed. Approximately. + a few seconds of measurement error that is, if it should actually be 1, it will give 1.2.
{
	float PathLength; 
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this->GetWorld());
	
	NavSystem->GetPathLength(PathStart, PathEnd, PathLength);

	// Length 

	return PathLength / (GetCharacterMovement()->MaxWalkSpeed / 4);
}



// Interactions

void ATerraCharacter::ChangeCharacterInteractionStatus(bool bFunctionType)
{
	bIsInteracted = bFunctionType;

	if (bFunctionType)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);

		GEngine->AddOnScreenDebugMessage(208, 1.0f, FColor::Emerald, "Minusspeed");
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	
}