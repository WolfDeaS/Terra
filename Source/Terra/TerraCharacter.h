// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProgressionComponent.h"
#include "InventoryComponent.h"
#include "StatusComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WHUD.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TerraCharacter.generated.h"

class ATimeActor;
class ASocialCell;
class AInteractionMark;
class AInteractableActor;

UENUM(BlueprintType)
enum class ECharacterStatus
{
	Relax UMETA(DisplayName = "Relaxing"),
	Working UMETA(DisplayName = "Working")
};

UCLASS(Blueprintable)
class ATerraCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATerraCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Init();
	void InitProgressionComponent();
	void InitInventoryComponent();
	void InitStatusComponent();
	void UpdateCharacterMovement();
	float CalculatePathDuration(const FVector& PathStart, const FVector& PathEnd);



	UFUNCTION(BlueprintCallable)
	void TargetActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	static AActor* FindClosetstActorFromArray(TArray<AActor*> ActorArray, AActor* TargetActor);

	// Interactions

	// bFunction type: 0 - End Interaction, 1 - Began Interaction
	void ChangeCharacterInteractionStatus(bool bFunctionType);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere)
	class UProgressionComponent* ProgressionComponent;

	UPROPERTY(EditAnywhere)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStatusComponent* StatusComponent;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Attributes"));
	UDataTable* DT_Attributes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Skills"));
	UDataTable* DT_Skills;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Items"));
	UDataTable* DT_Items;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Herbs"));
	UDataTable* DT_Herbs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Beverage"));
	UDataTable* DT_Beverage;
	

	// Тут задаються стандартні значення різних модифікаторів. Вони потім корегується скілами та іншим.
	// Наприклад: Яке карго стандартне, та інше.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "Modifiers"));
	TMap<FName, float> Modifiers;

	// Social Cell
	TMap<FName, float> RequestMap;
	FName Activity;

	ATimeActor* TimeActor;
	ASocialCell* SocialCell;

	TMap<FName, TArray<AInteractableActor*>> InteractableActors;
	TMap<FName, TArray<AInteractionMark*>> MarksActor;

	TArray<AActor*> LocalArray;

	// Interactions
	FTimerHandle InteractionTimerHandle;
	FTimerHandle InteractionAddRotTimerHandle;
	bool bIsInteracted;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};

