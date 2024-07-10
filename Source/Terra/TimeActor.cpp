// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeActor.h"

// Sets default values
ATimeActor::ATimeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimeActor::BeginPlay()
{
	Super::BeginPlay();

	OneInGameSecondValue = float(DayLength / DayLengthInRealSeconds);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([=, this]() { OneInGameSecond(); }), OneInGameSecondValue, true);

	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractableActor::StaticClass(), FoundActors);

	for (auto& LocalChest : FoundActors)
	{
		if (Cast<AInteractableActor>(LocalChest)->InteractionComponent->InteractID == EInteractID::Chest)
		{
			Chests.Add(Cast<AInteractableActor>(LocalChest));
		}
	}
}

void ATimeActor::OneInGameSecond()
{
	if (TimeMinute + 1 > 1440)
	{
		TimeMinute = 1;
	}
	else
	{
		TimeMinute++;
	}
	float LocalHour = TimeMinute / 60;
	LocalHour = FMath::Floor(LocalHour);

	TimeHour = LocalHour;
	uint8 LocalMinute = TimeMinute - (TimeHour * 60);

	FString LocalString;
	LocalString.Append(FString::FromInt(TimeHour));
	LocalString.Append(":");
	LocalString.Append(FString::FromInt(LocalMinute));


	if (!Characters.IsEmpty())
	{
		for (auto& LocalCharacter : Characters)
		{
			LocalCharacter->InventoryComponent->OnSecondCalculations();
			LocalCharacter->StatusComponent->OnSecondCalculations();
		}
	}
	for (auto& LocalThrowedItem : ThrowedItems)
	{
		LocalThrowedItem->InteractionComponent->OnSecondCalculations();
	}
	for (auto& LocalChest : Chests)
	{
		LocalChest->InventoryComponent->OnSecondCalculations();
	}
}

void ATimeActor::AddCharacterToArray(ATerraCharacter* TerraCharacter)
{
	Characters.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerraCharacter::StaticClass(), FoundActors);

	for (auto& LocalCharacter : FoundActors)
	{
		Characters.Add(Cast<ATerraCharacter>(LocalCharacter));
	}
}