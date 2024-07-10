// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialCell.h"
#include "TimeActor.h"
#include "TerraCharacter.h"

// Sets default values
ASocialCell::ASocialCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocalSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Local Scene Component"));
	RootComponent = LocalSceneComponent;

	LocalCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Local Collision Mesh"));
	LocalCollisionBox->SetupAttachment(LocalSceneComponent);
}

// Called when the game starts or when spawned
void ASocialCell::BeginPlay()
{
	Super::BeginPlay();
	
	SCType = ESCType::Forest;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() { InitCell(); }, 0.001f, false);
}

// Called every frame
void ASocialCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RequestMap.Contains("Hunger"))
	{
		GEngine->AddOnScreenDebugMessage(8191, 0.1f, FColor::Purple, FString::SanitizeFloat(*RequestMap.Find("Hunger")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(8191, 0.1f, FColor::Purple, "0.0");
	}
	
}

void ASocialCell::InitCell()
{
	TArray<AActor*> ActorsOverlap;
	LocalCollisionBox->GetOverlappingActors(ActorsOverlap, ATerraCharacter::StaticClass());

	for (auto& LocalCharacter : ActorsOverlap)
	{
		Characters.Add(Cast<ATerraCharacter>(LocalCharacter));
		Cast<ATerraCharacter>(LocalCharacter)->SocialCell = this;
	}

	LocalCollisionBox->GetOverlappingActors(ActorsOverlap, AInteractableActor::StaticClass());

	for (auto& LocalInteractableActor : ActorsOverlap)
	{
		TArray<AInteractableActor*> LocalArray;
		if (InteractableActors.Contains(*UEnum::GetValueAsString(Cast<AInteractableActor>(LocalInteractableActor)->InteractionComponent->InteractID)))
		{
			LocalArray = *InteractableActors.Find(*UEnum::GetValueAsString(Cast<AInteractableActor>(LocalInteractableActor)->InteractionComponent->InteractID));
		}
		
		LocalArray.Add(Cast<AInteractableActor>(LocalInteractableActor));
		InteractableActors.Add(*UEnum::GetValueAsString(Cast<AInteractableActor>(LocalInteractableActor)->InteractionComponent->InteractID), LocalArray);
	} 

	CharactersOutOfActivities = Characters;
} 

void ASocialCell::CalculateAndAddToRequestStatus(FName Status)
{
	RequestMap.Remove(Status);

	float LocalValue = 0.0f;
	FString LocalString;
	FString AnotherLocalString;

	for (auto& LocalCharacter : Characters)
	{
		LocalString = Status.ToString();
		LocalString.Append("Decay");
		AnotherLocalString = Status.ToString();
		AnotherLocalString.Append("DebuffsStarts");
		LocalValue += (LocalCharacter->TimeActor->DayLengthInRealSeconds * *LocalCharacter->Modifiers.Find(*LocalString)) + *LocalCharacter->Modifiers.Find(*AnotherLocalString);
	}


	if (GiveRequestMap.Contains(Status))
	{
		LocalValue -= *RequestMap.Find(Status);
	}

	RequestMap.Add(Status, LocalValue);
}

void ASocialCell::GiveTasks()
{
	bool bHaveMark;
	bHaveMark = false;
	UDataTable* LocalDT;
	LocalDT = Characters[0]->StatusComponent->DT_Statuses;
	for (auto& Pair : RequestMap)
	{
		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = LocalDT->FindRow<FSStatusCreation>(Pair.Key, TEXT("none"), false);

		if (StatusDataFromDT)
		{
			for (auto& LocalAcitivity : StatusDataFromDT->ActivitiesToRestore)
			{
				FSActivitiesCreation* ActivitiesDataFromDT;
				ActivitiesDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(LocalAcitivity, TEXT("none"), false);

				for (auto& LocalMarkEnum : ActivitiesDataFromDT->MarkForActivities)
				{
					if (MarksActor.Contains(*UEnum::GetValueAsString(LocalMarkEnum)))
					{
						bHaveMark = true;
					}
				}
			}
			if (bHaveMark)
			{
				
			}
			else // Roaming
			{
				if (StatusDataFromDT->bIsRoamingNeededToRestore)
				{
					TArray<ATerraCharacter*> ArrayToDelete;

					for (auto& LocalCharacter :CharactersOutOfActivities)
					{
						ArrayToDelete.Add(LocalCharacter);

						UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityName", "RoamingForStatus");
						UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityInfo", Pair.Key);
						UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsEnum("GlobalCharacterStatusKey", (uint8)ECharacterStatus::Working);

						TArray<ATerraCharacter*> LocalCharactersArray;
						if (CharactersActivities.Contains("RoamingForStatus"))
						{
							LocalCharactersArray = *CharactersActivities.Find("RoamingForStatus");
							LocalCharactersArray.Add(LocalCharacter);
							CharactersActivities.Add("RoamingForStatus", LocalCharactersArray);
						}
						else
						{
							LocalCharactersArray.Add(LocalCharacter);
							CharactersActivities.Add("RoamingForStatus", LocalCharactersArray);
						}

						LocalCharacter->RequestMap.Add(Pair.Key, CharactersOutOfActivities.Num());
					}
					for (auto& LocalCharacter : ArrayToDelete)
					{
						CharactersOutOfActivities.Remove(LocalCharacter);
					}

					//FindBetterCharacterForActivity("RoamingForStatus", Pair.Key);
				}
			}
		}
	}
}

//TODO, cause now it only for Roaming
ATerraCharacter* ASocialCell::FindBetterCharacterForActivity(FName Activity, FName AdditionalInfo)
{
	ATerraCharacter* OutputCharacter;
	OutputCharacter = Characters[0];

	if (Activity == "RoamingForStatus")
	{
		UDataTable* LocalDT;
		LocalDT = Characters[0]->StatusComponent->DT_Statuses;

		FSStatusCreation* StatusDataFromDT;
		StatusDataFromDT = LocalDT->FindRow<FSStatusCreation>(AdditionalInfo, TEXT("none"), false);

		FSActivitiesCreation* ActivitiesDataFromDT;
		ActivitiesDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(Activity, TEXT("none"), false);

		TArray<FName> ShareActivities;

		for (auto& LocalStatusActivity : StatusDataFromDT->ActivitiesToRestore)
		{
			for (auto& LocalActivityActivity : ActivitiesDataFromDT->PossibleActivities.Find(SCType)->Array)
			{
				if (LocalStatusActivity == LocalActivityActivity)
				{
					ShareActivities.Add(LocalActivityActivity);
					break;
				}
			}
		}

		TMap<FName, float> ShareAttributesPriority;
		for (auto& LocalActivity : ShareActivities)
		{
			FSActivitiesCreation* LocalActivitiesDataFromDT;
			LocalActivitiesDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);
			for (auto& Pair : LocalActivitiesDataFromDT->SkillsPriority)
			{
				if (ShareAttributesPriority.Contains(Pair.Key))
				{
					ShareAttributesPriority.Add(Pair.Key, *ShareAttributesPriority.Find(Pair.Key) + Pair.Value);
				}
				else
				{
					ShareAttributesPriority.Add(Pair.Key, Pair.Value);
				}
			}
		}

		float LocalValue = 0.0f;
		float PREVValue = -1.0f;
		for (auto& LocalCharacter : Characters)
		{
			for (auto& LocalPriority : ShareAttributesPriority)
			{
				LocalValue = LocalCharacter->ProgressionComponent->FindAttribute(LocalPriority.Key)->Level * LocalPriority.Value;
			}

			if (LocalValue > PREVValue)
			{
				PREVValue = LocalValue;
				OutputCharacter = LocalCharacter;
			}
		}
	}

	return OutputCharacter;
}