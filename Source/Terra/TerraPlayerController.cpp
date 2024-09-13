// Copyright Epic Games, Inc. All Rights Reserved.

#include "TerraPlayerController.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATerraPlayerController::ATerraPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;

	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(FName("Path Following Component"));
	PathFollowingComponent->RegisterComponent();

	CharacterMenuClass = nullptr;
	CharacterMenu = nullptr;

}

void ATerraPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	Character = Cast<ATerraCharacter>(this->GetPawn());

	CreateMainUI();
	
}

void ATerraPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ATerraPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ATerraPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ATerraPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ATerraPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ATerraPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ATerraPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ATerraPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ATerraPlayerController::OnTouchReleased);

		EnhancedInputComponent->BindAction(SetMoveAction, ETriggerEvent::Triggered, this, &ATerraPlayerController::OnMove);

		EnhancedInputComponent->BindAction(ChangeInventoryStatus, ETriggerEvent::Started, this, &ATerraPlayerController::ChangeMainUIStatus);

		EnhancedInputComponent->BindAction(ESC_Button, ETriggerEvent::Started, this, &ATerraPlayerController::CloseInterfaceWithESC);

		EnhancedInputComponent->BindAction(ThrowItem_Button, ETriggerEvent::Started, this, &ATerraPlayerController::ThrowItem);
		EnhancedInputComponent->BindAction(UseItem_Button, ETriggerEvent::Started, this, &ATerraPlayerController::UseItem);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATerraPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ATerraPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);

		if (MovedToInteractableActor)
		{
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(0);
		}
		if (InteractableActor)
		{
			FVector PawnLocation;
			PawnLocation = this->GetPawn()->GetActorLocation();
			FVector LocalNearestPointOfInteractableActor;
			MovedToInteractableActor = InteractableActor;
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::GetClosestPointOnCollision(PawnLocation, LocalNearestPointOfInteractableActor);
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, LocalNearestPointOfInteractableActor);
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(1);
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(4);
			InteractAfterReach = 0;
		}
	} 
}

void ATerraPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		GEngine->AddOnScreenDebugMessage(208129, 1.0f, FColor::Emerald, "Go To");
		// We move there and spawn some particles
		if (InteractableActor)
		{
			FVector PawnLocation;
			PawnLocation = this->GetPawn()->GetActorLocation();
			FVector LocalNearestPointOfInteractableActor; 
			MovedToInteractableActor = InteractableActor;
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::GetClosestPointOnCollision(PawnLocation, LocalNearestPointOfInteractableActor);
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, LocalNearestPointOfInteractableActor);
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(1);
			MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(4);
			InteractAfterReach = 1;
			GEngine->AddOnScreenDebugMessage(208122, 1.0f, FColor::Emerald, "IA Go To");
		}
		else if (InteractableRiver)
		{
			FVector PawnLocation;
			PawnLocation = this->GetPawn()->GetActorLocation();
			FVector LocalNearestPointOfInteractableActor;
			MovedToInteractableRiver = InteractableRiver;

			FVector StartPos = Character->GetActorLocation();
			FVector EndPos = CachedDestination;
			float LocalAngleRadians = UKismetMathLibrary::FindLookAtRotation(StartPos, EndPos).Yaw * (PI / 180);
			
			int32 LocalStatusOfSteps = 0;

			FVector LocalPos = StartPos;
			FVector PREVPos;
			float x;
			float y;

			// True: To Start Pos
			// False: To End Pos
			//bool StepDirection = false;

			while (LocalStatusOfSteps < STEPS_FOR_RIVER_FINDING.Num())
			{
				PREVPos = LocalPos;

				x = LocalPos.X + STEPS_FOR_RIVER_FINDING[LocalStatusOfSteps] * cos(LocalAngleRadians);
				y = LocalPos.Y + STEPS_FOR_RIVER_FINDING[LocalStatusOfSteps] * sin(LocalAngleRadians);
				UE_LOG(LogTemplateCharacter, Error, TEXT("Move To River: Forward"));
				

				LocalPos = FVector(x, y, 10000.0f);

				FHitResult LineTraceResult;
				const FName TraceTag("DebugTrace");

				GetWorld()->DebugDrawTraceTag = TraceTag;

				FCollisionQueryParams CollisionParams;
				CollisionParams.TraceTag = TraceTag;

				if (GetWorld()->LineTraceSingleByChannel(LineTraceResult, LocalPos, FVector(x, y, -10000.0f), ECollisionChannel::ECC_GameTraceChannel1, CollisionParams, FCollisionResponseParams()))
				{

					FString LocalName;
					LocalName = LineTraceResult.GetActor()->GetName();
					GEngine->AddOnScreenDebugMessage(2091, 1.0f, FColor::Cyan, LocalName);

					if (Cast<AInteractableRiver>(LineTraceResult.GetActor()) == MovedToInteractableRiver)
					{
						LocalPos = PREVPos;

						LocalStatusOfSteps++;

						if (LocalStatusOfSteps == STEPS_FOR_RIVER_FINDING.Num())
						{
							LocalPos = FVector(LocalPos.X, LocalPos.Y, LineTraceResult.Location.Z);
						}

						UE_LOG(LogTemplateCharacter, Error, TEXT("Move To River: LocalStatusOfSteps NextLevel Forward"));
					}
				}
			}
			
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, LocalPos);
			InteractAfterReach = 1;
		}
		else
		{
			if (MovedToInteractableActor || MovedToInteractableRiver)
			{
				RemoveMovedToInteractableActor();
				GEngine->AddOnScreenDebugMessage(2084, 1.0f, FColor::Emerald, "OnSetDestinationReleased: RemoveMovedToInteractableActor");
			}
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ATerraPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ATerraPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ATerraPlayerController::OnMove(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		StopMovement();
		const FVector2D InputVector = Value.Get<FVector2D>();
		FVector LocalVector;
		LocalVector = FVector(InputVector.X, InputVector.Y, 0.0f);
		ControlledPawn->AddMovementInput(LocalVector, 1.0f, false);

		if (MovedToInteractableActor)
		{
			RemoveMovedToInteractableActor();
		}
	}
}

void ATerraPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(2081232, 1.0f, FColor::Emerald, Character->GetActorLocation().ToString());
	FVector MousePositionLocation;
	FVector MousePositionDirection;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectMousePositionToWorld(MousePositionLocation, MousePositionDirection);


	MousePositionDirection *= 100000.0f;
	MousePositionDirection += MousePositionLocation;

	FHitResult LineTraceResult;
	FCollisionQueryParams CollisionParams;

	if (GetWorld()->LineTraceSingleByChannel(LineTraceResult, MousePositionLocation, MousePositionDirection, ECollisionChannel::ECC_GameTraceChannel1, CollisionParams, FCollisionResponseParams()))
	{
		if (InteractableActor)
		{
			InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
		}

		if (LineTraceResult.GetActor()->IsA<AInteractableActor>())
		{
			InteractableActor = Cast<AInteractableActor>(LineTraceResult.GetActor());
			InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(1);
			GEngine->AddOnScreenDebugMessage(2081290, 1.0f, FColor::Emerald, "IA Detected");
		}
		else if(LineTraceResult.GetActor()->IsA<AInteractableRiver>())
		{
			InteractableRiver = Cast<AInteractableRiver>(LineTraceResult.GetActor());
			GEngine->AddOnScreenDebugMessage(2081291, 1.0f, FColor::Emerald, "River Detected");
		}
	}
	else
	{
		if (InteractableActor && !MovedToInteractableActor)
		{
			if (InteractableActor->IsA<AInteractableActor>())
			{
				InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
				InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(1);
			}
		}
		InteractableActor = NULL;
		InteractableRiver = NULL;
		GEngine->AddOnScreenDebugMessage(2085, 1.0f, FColor::Emerald, "Tick: Remove InteractableRiver and InteractableActor");
	}

	if ((MovedToInteractableActor || MovedToInteractableRiver) && InteractAfterReach)
	{
		if (MovedToInteractableActor)
		{
			if (MovedToInteractableActor->LocalCollisionBox->IsOverlappingActor(this->GetPawn()))
			{
				InteractAfterReach = 0;
				MovedToInteractableActor->InteractionComponent->OnInteractionBegan(Character);
				RemoveMovedToInteractableActor();
				if (InteractableActor)
				{
					InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
					InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(1);

					InteractableActor = NULL;
				}
			}
		}
		else if (MovedToInteractableRiver)
		{
			if (MovedToInteractableRiver->GetWaterBodyComponent()->IsOverlappingActor(this->GetPawn()))
			{
				InteractAfterReach = 0;
				MovedToInteractableRiver->InteractionComponent->OnInteractionBegan(Character);
				RemoveMovedToInteractableActor();
				if (InteractableRiver)
				{
					InteractableRiver = NULL;

					GEngine->AddOnScreenDebugMessage(2084, 1.0f, FColor::Emerald, "Tick: Remove InteractableRiver");
				}
			}
		}
	}
	if ((MovedToInteractableActor || MovedToInteractableRiver) && !InteractAfterReach)
	{
		if (MovedToInteractableActor)
		{
			if (MovedToInteractableActor->LocalCollisionBox->IsOverlappingActor(this->GetPawn()))
			{
				RemoveMovedToInteractableActor();
				if (InteractableActor)
				{
					InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
					InteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(1);
					InteractableActor = NULL;
				}
			}
		}
		else if (MovedToInteractableRiver)
		{
			if (MovedToInteractableRiver->GetWaterBodyComponent()->IsOverlappingActor(this->GetPawn()))
			{
				InteractAfterReach = 0;
				MovedToInteractableRiver->InteractionComponent->OnInteractionBegan(Character);
				RemoveMovedToInteractableActor();
				if (InteractableRiver)
				{
					InteractableRiver = NULL;
					GEngine->AddOnScreenDebugMessage(2084, 1.0f, FColor::Emerald, "Tick: Remove MovedToInteractableActor");
				}
			}
		}
	}
}

void ATerraPlayerController::RemoveMovedToInteractableActor()
{
	if (MovedToInteractableActor)
	{
		MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetRenderCustomDepth(0);
		MovedToInteractableActor->LocalStaticMesh->UPrimitiveComponent::SetCustomDepthStencilValue(0);
		MovedToInteractableActor = NULL;

		GEngine->AddOnScreenDebugMessage(2082, 1.0f, FColor::Emerald, "RemoveMovedToInteractableActor: MovedToInteractableActor"); 
	}
	if (MovedToInteractableActor)
	{
		MovedToInteractableRiver = NULL;
		GEngine->AddOnScreenDebugMessage(2083, 1.0f, FColor::Emerald, "RemoveMovedToInteractableActor: MovedToInteractableActor");
	}
	
}

void ATerraPlayerController::SetPausedGameStatus(bool PausedStatus)
{
	APlayerController::SetPause(PausedStatus);
}

void ATerraPlayerController::ThrowItem()
{
	if(CharacterMenu->SelectedItemStruct.ID != "None")
	{
		Character->InventoryComponent->ThrowItem(CharacterMenu->SelectedItemStruct);
		CharacterMenu->UnSelectedItem();
		CharacterMenu->CreateInventoryWB(Character->InventoryComponent->Inventory);
	}
}

void ATerraPlayerController::UseItem()
{
	if (CharacterMenu->bIsChestOpen)
	{
		if (CharacterMenu->bIsChest)
		{
			CharacterMenu->Character->InventoryComponent->AddItemToInventory(CharacterMenu->SelectedItemStruct, 1);
			CharacterMenu->InteractableActor->InventoryComponent->RemoveItem(CharacterMenu->SelectedItemStruct, 1, 1);
			CharacterMenu->CreateObjectInventoryWB();
			CharacterMenu->UpdateStats();
		}
		else
		{
			CharacterMenu->Character->InventoryComponent->RemoveItem(CharacterMenu->SelectedItemStruct, 1);
			CharacterMenu->InteractableActor->InventoryComponent->AddItemToInventory(CharacterMenu->SelectedItemStruct, 1, 1);
			CharacterMenu->CreateInventoryWB(Character->InventoryComponent->Inventory);
			CharacterMenu->UpdateStats();
		}
	}
	else
	{
		if (CharacterMenu->SelectedItemStruct.ID != "None")
		{
			Character->InventoryComponent->UseItem(CharacterMenu->SelectedItemStruct);
			CharacterMenu->UnSelectedItem();
			CharacterMenu->CreateInventoryWB(Character->InventoryComponent->Inventory);
		}
	}
}


// Interface

void ATerraPlayerController::CreateMainUI()
{
	if (CharacterMenuClass)
	{
		CharacterMenu = CreateWidget<UWCharacterMenu>(this, CharacterMenuClass);
		check(CharacterMenu);
		CharacterMenu->Character = Character;
		CharacterMenu->AddToPlayerScreen();

		CharacterMenu->StatusUI = 0;
		CharacterMenu->ChangeUIStatus();
	}
	if (HUDClass)
	{
		HUD = CreateWidget<UWHUD>(this, HUDClass);
		check(HUD);
		HUD->Character = Character;
		HUD->AddToPlayerScreen();

		Character->InitStatusComponent();
		HUD->Statuses = Character->StatusComponent->DT_Statuses->GetRowNames();
		HUD->Init();
	}
}

void ATerraPlayerController::ChangeMainUIStatus()
{
	CharacterMenu->ChangeUIStatus();
}

void ATerraPlayerController::CloseInterfaceWithESC()
{
	if (CharacterMenu->StatusUI == 0)
	{
		if (CharacterMenu->SelectedInventorySizeBox)
		{
			CharacterMenu->UnSelectedItem();
			return;
		}
		APlayerController::SetPause(false);
		CharacterMenu->ChangeUIStatus();
		return;
	}
}

FVector ATerraPlayerController::GetClosestPointOnSpline(USplineComponent* Spline, const FVector& Location)
{

	float ClosestDistance = FLT_MAX;
	int32 ClosestKey = 0;

	for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); ++i)
	{
		FVector SplinePoint = Spline->GetWorldLocationAtSplinePoint(i);
		float Distance = FVector::Dist(Location, SplinePoint);
		UE_LOG(LogTemplateCharacter, Error, TEXT("Spline point Distance %d: %s"), i, *SplinePoint.ToString());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestKey = i;
		}
	}

	return Spline->GetWorldLocationAtSplinePoint(ClosestKey);
}