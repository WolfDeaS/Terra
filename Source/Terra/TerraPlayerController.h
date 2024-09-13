// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InteractableActor.h"
#include "TerraCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Tasks/AITask_MoveTo.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/WCharacterMenu.h"
#include "InteractableRiver.h"
#include "Widgets/WHUD.h"
#include "WaterSplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TerraPlayerController.generated.h"



/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class ATerraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATerraPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** Визначає базову швикдість персонажів */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float BaseValueSpeed;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeInventoryStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ESC_Button;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowItem_Button;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UseItem_Button;

	UPathFollowingComponent* PathFollowingComponent;

	AInteractableRiver* InteractableRiver;
	AInteractableActor* InteractableActor;

	AInteractableRiver* MovedToInteractableRiver;
	AInteractableActor* MovedToInteractableActor;

	ATerraCharacter* Character;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<class UWCharacterMenu> CharacterMenuClass;
	UPROPERTY()
	class UWCharacterMenu* CharacterMenu;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<class UWHUD> HUDClass;
	UPROPERTY()
	class UWHUD* HUD;
	

	bool InteractAfterReach;

	void SetPausedGameStatus(bool PausedStatus);
	
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnMove(const FInputActionValue& Value);
	
	void ThrowItem();
	void UseItem();
	void RemoveMovedToInteractableActor();

	//Interface

	void CreateMainUI();
	void ChangeMainUIStatus();
	void CloseInterfaceWithESC();

	static FVector GetClosestPointOnSpline(USplineComponent* Spline, const FVector& Location);

private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	const TArray<float> STEPS_FOR_RIVER_FINDING = { 100.0f,10.0f,1.0f };

	//const char* SetYAction;
	//const char* SetXAction;
};


