// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "InteractionComponent.h"
#include "StatusComponent.h"
#include "InteractionMark.h"
#include "InteractableActor.h"
#include "SocialCell.generated.h"

UENUM(BlueprintType)
enum ESCType
{
	Forest,
	City,
};

USTRUCT(BlueprintType)
struct FSNameArrays : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Array;
};

USTRUCT(BlueprintType)
struct FSActivitiesCreation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Tags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TEnumAsByte<ESCType>, FSNameArrays> PossibleActivities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, float> SkillsPriority;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<EMarkType>> MarkForActivities;
};

UCLASS()
class TERRA_API ASocialCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// SC same as SocialCell
	ASocialCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InitCell();
	void CalculateAndAddToRequestStatus(FName Status);
	
	TMap<FName, TArray<AInteractionMark*>> UniteMarks(TMap<FName, TArray<AInteractionMark*>> FirstMarksArray, TMap<FName, TArray<AInteractionMark*>> AnotherMarksArray);


	/// Searching ///

	ATerraCharacter* FindBetterCharacterForActivity(FName Activity, FName AdditionalInfo = "None");
	ATerraCharacter* GetSlowestCharacter();
	float GetStatusConsumeAtSecond(FName Status);
	float GetStatusConsueAtDay(FName Status);
	FName FindActivityByMark(TEnumAsByte<EMarkType> MarkType);
	TArray<FName> GetMarksIDForRestoreStatus(FName Status);
	float GetEfficencyMarkValue(AInteractionMark* LocalMark, bool bFunctionType, FName Status);
	

	//////

	/// Interaction with Characters /// 

	void GiveTasks();

	bool IsCharacterHaveRequestItems(ATerraCharacter* LocalCharacter);

	void ChangeCharacterWorkStatus(uint8 FunctionType = 0, bool bDeleteItemFromCharactersArray = false , ATerraCharacter* LocalCharacter = nullptr, UObject* ObjectForWorking = nullptr, FName ActivityInfo = "None", FName AddInfo = "None");

	//////////////////////////////////

	UFUNCTION(BlueprintCallable)
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere)
	USceneComponent* LocalSceneComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UBoxComponent* LocalCollisionBox;

	TEnumAsByte<ESCType> SCType;

	TMap<FName, TArray<AInteractableActor*>> InteractableActors;
	TMap<FName, TArray<AInteractionMark*>> MarksActor;
	
	TMap<FName, TArray<EInteractID>> ActorsTypeThatResotreStatuses;

	TMap<FName, float> RequestMap;
	TMap<FName, float> GiveRequestMap;

	TArray<ATerraCharacter*> Characters;
	TMap<FName, TArray<ATerraCharacter*>> CharactersActivities;
	TArray<ATerraCharacter*> CharactersOutOfActivities;

	UPROPERTY(EditAnywhere)
	UDataTable* DT_Acitivites;

	UPROPERTY(EditAnywhere)
	UDataTable* DT_Herb;

	UPROPERTY(EditAnywhere)
	UDataTable* DT_Items;
};
