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

	if (InteractableActors.Contains("Chest"))
	{
		GEngine->AddOnScreenDebugMessage(90771, 1.0f, FColor::Cyan, FString::FromInt(InteractableActors.Find("Chest")->Num()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(90771, 1.0f, FColor::Cyan, "0");
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

float ASocialCell::GetStatusConsumeAtSecond(FName Status)
{
	float LocalValue = 0.0f;
	FString LocalString;

	for (auto& LocalCharacter : Characters)
	{
		LocalString = Status.ToString();
		LocalString.Append("Decay");
		LocalValue += *LocalCharacter->Modifiers.Find(*LocalString);
	}

	return LocalValue;
}

float ASocialCell::GetStatusConsueAtDay(FName Status)
{
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

	return LocalValue;
}

/// Interactions with characters ///

void ASocialCell::GiveTasks()
{
	bool bHaveMark;
	bHaveMark = false;
	UDataTable* LocalDT;
	LocalDT = Characters[0]->StatusComponent->DT_Statuses;
	TArray<FName> MarkNames;
	TArray<FString> LocalStringArray;
	FString LocalString;
	MarksActor.GenerateKeyArray(MarkNames);
	
	TMap<FName, TArray<AInteractionMark*>> LocalIMArray;

	// Key - Requsest id, i.e.: status, weapons etc.
	for (auto& Pair : RequestMap)
	{
		GEngine->AddOnScreenDebugMessage(90777, 1.0f, FColor::Yellow, Pair.Key.ToString());

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

					for (auto& LocalName : MarkNames)
					{
						LocalName.ToString().ParseIntoArray(LocalStringArray, TEXT(";"));
						LocalString = LocalStringArray[0];
					
						UEnum::GetValueAsString(LocalMarkEnum).ParseIntoArray(LocalStringArray, TEXT("s"));

						if (LocalString == LocalStringArray[0])
						{
							bHaveMark = true;

							LocalIMArray.Add(LocalName, *MarksActor.Find(LocalName));
						}
					} 
				}
			}
			if (bHaveMark)
			{
				// Weight of Mark, which higher number -> that mark will be chosen
				float LocalValue = -10000.0f;
				float LocalGrowthStatusInSecond = -1.0f;
				float LocalDecayStatusInSecond = -1.0f;
				float LocalGrowthStatus = 0.0f;
				AInteractionMark* OutputMark = nullptr;
				FName LocalMarkID;
				FSHerbCreation* HerbDataFromDT;
				FSItemCreation* ItemDataFromDT;
				
				TMap<FName, float> LocalModifierBonuses;
				float Time = 0.0f;

				float GrowthStatusInSecond = -1.0f;
				float DecayStatusInSecond = -1.0f;

				
				/// TODO: ONLY FOR HERBS

				// Key = Mark ID with Add Info
				// Value = Marks Array
				for (auto& LocalIMPair : LocalIMArray)
				{
					// We find LocalMark ID, and find HerbDataFromDT just one time for all ID

					LocalIMPair.Key.ToString().ParseIntoArray(LocalStringArray, TEXT(";"));
					LocalMarkID = *LocalStringArray[1];
					
					HerbDataFromDT = DT_Herb->FindRow<FSHerbCreation>(LocalMarkID, TEXT("none"), false);


					for (auto& LocalMark : LocalIMPair.Value)
					{
						Time = GetSlowestCharacter()->CalculatePathDuration(this->GetActorLocation(), LocalMark->GetActorLocation());

						for (auto& LocalItemPair : HerbDataFromDT->ItemsGiven)
						{
							ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(LocalItemPair.Key, TEXT("none"), false);
							LocalModifierBonuses = Characters[0]->InventoryComponent->CalculateItemModifiersDurabilityBasedOnSeconds(LocalItemPair.Key,Time);

							if (LocalModifierBonuses.Contains(Pair.Key))
							{
								LocalGrowthStatus += *LocalModifierBonuses.Find(Pair.Key) * LocalItemPair.Value;
							}
						}
						LocalGrowthStatusInSecond = LocalGrowthStatus / Time;
						LocalDecayStatusInSecond = ((Pair.Value + (Time * GetStatusConsumeAtSecond(Pair.Key))) * 1.25) / Characters[0]->TimeActor->DayLengthInRealSeconds;

						if (LocalGrowthStatusInSecond - LocalDecayStatusInSecond > LocalValue)
						{
							LocalValue = LocalGrowthStatusInSecond - LocalDecayStatusInSecond;
							OutputMark = LocalMark;
							GrowthStatusInSecond = LocalGrowthStatusInSecond;
							DecayStatusInSecond = LocalDecayStatusInSecond;
						}
					}
				}

				UE_LOG(LogTemp, Warning, TEXT("Give Task, LocalGrowthStatusInSecond: %f"), LocalGrowthStatusInSecond);
				UE_LOG(LogTemp, Warning, TEXT("Give Task, LocalDecayStatusInSecond: %f"), LocalDecayStatusInSecond);
				UE_LOG(LogTemp, Warning, TEXT("Give Task, LocalValue: %f"), LocalValue);

				ATerraCharacter* LocalCharacter;
				FName LocalActivityName = FindActivityByMark(OutputMark->MarkType);

				if (LocalValue > 0)
				{
					LocalCharacter = FindBetterCharacterForActivity(LocalActivityName);
					LocalCharacter->RequestMap.Add(Pair.Key, Pair.Value);

					
					ChangeCharacterWorkStatus(0, true, LocalCharacter, OutputMark, LocalActivityName);

					const float NumOfApproaches = FMath::CeilToFloat(Pair.Value / LocalGrowthStatus);
					UE_LOG(LogTemp, Warning, TEXT("Give Task, CeilToFloat: %f"), NumOfApproaches);
					UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditional", NumOfApproaches);
				}
				else
				{
					const int32 ArrayLength = CharactersOutOfActivities.Num();
					TArray<ATerraCharacter*> MarkCharacters;

					for (int32 i = 0; i < ArrayLength; i++)
					{
						LocalCharacter = FindBetterCharacterForActivity(LocalActivityName);
						ChangeCharacterWorkStatus(0, true, LocalCharacter, OutputMark, LocalActivityName);
						MarkCharacters.Add(LocalCharacter);

						if (i != 0)
						{
							LocalValue += LocalGrowthStatusInSecond;
						}
						GEngine->AddOnScreenDebugMessage(i, 10.0f, FColor::Red, FString::FromInt(LocalValue));
						if (LocalValue > 0)
						{
							break;
						}
					}

					const float RequestValue = Pair.Value / MarkCharacters.Num();
					const float NumOfApproaches = FMath::CeilToFloat(RequestValue / LocalGrowthStatus);

					

					for (auto& LocalCharacterArray : MarkCharacters)
					{
						if (LocalCharacterArray->RequestMap.Contains(Pair.Key))
						{
							LocalCharacterArray->RequestMap.Add(Pair.Key, LocalCharacterArray->RequestMap.Contains(Pair.Key) + RequestValue);
						}
						else
						{
							LocalCharacterArray->RequestMap.Add(Pair.Key, RequestValue);
						}

						UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacterArray)->SetValueAsInt("ActivityIntAdditional", NumOfApproaches);
					}
				}
			}
			else // Roaming
			{
				if (StatusDataFromDT->bIsRoamingNeededToRestore)
				{
					TArray<ATerraCharacter*> ArrayToDelete;
					float LocalValue = -1.0f;
					AInteractionMark* LocalIM = nullptr;
					TArray<AInteractionMark*> NearestToObjectMarks;
					TArray<AActor*> LocalIMForStatus;

					TArray<AActor*> LocalActors;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimeActor::StaticClass(), LocalActors);
					ATimeActor* LocalTimeActor = Cast<ATimeActor>(LocalActors[0]);

					const TArray<AInteractionMark*> AllIMForStatus = LocalTimeActor->GetAllMarksForStatus(Pair.Key);
					
					// Casting to AActor*, cause i need it
					for (auto& LocalMark : AllIMForStatus)
					{
						LocalIMForStatus.Add(Cast<AActor>(LocalMark));
					}

					// We take 5 nearest for future calculations
					for (size_t i = 0; i < 5; i++)
					{
						AActor* LocalActor = ATerraCharacter::FindClosetstActorFromArray(LocalIMForStatus, this);
						NearestToObjectMarks.Add(Cast<AInteractionMark>(LocalActor));
						LocalIMForStatus.Remove(LocalActor);

						if (LocalIMForStatus.IsEmpty())
						{
							break;
						}
					}

					

					// Find more effecient Mark
					for (auto& LocalMark : NearestToObjectMarks)
					{
						if (GetEfficencyMarkValue(LocalMark, 1, Pair.Key) > LocalValue)
						{
							LocalValue = GetEfficencyMarkValue(LocalMark, 1, Pair.Key);
							LocalIM = LocalMark;
							UE_LOG(LogTemp, Warning, TEXT("Give Task, Local Mark: Add"));
							UE_LOG(LogTemp, Warning, TEXT("Give Task, Local Mark: LocalValue, %f"), LocalValue);
						}
					}

					// If we couldnt find effecient mark, we find effecient from all across the map
					if (LocalValue <= 0)
					{
						for (auto& LocalMark : AllIMForStatus)
						{
							if (GetEfficencyMarkValue(LocalMark, 1, Pair.Key) > LocalValue)
							{
								LocalValue = GetEfficencyMarkValue(LocalMark, 1, Pair.Key);
								LocalIM = LocalMark;
							}
						}
					}

					for (auto& LocalCharacter : CharactersOutOfActivities)
					{
						ArrayToDelete.Add(LocalCharacter);

						ChangeCharacterWorkStatus(0, false, LocalCharacter, Cast<AActor>(LocalIM), "RoamingForStatus", Pair.Key);

						// Evenly distribute requests
						LocalCharacter->RequestMap.Add(Pair.Key, Pair.Value / CharactersOutOfActivities.Num());
					}
					for (auto& LocalCharacter : ArrayToDelete)
					{
						CharactersOutOfActivities.Remove(LocalCharacter);
					}
				}
			}
		}
	}
}

//bFunctionType to make Character: 0 - Working, 1 - ReturnToSC 2 - Rest.
//bDeleteItemFromCharactersArray - Means delete character from CharactersOutOfActivities if bFunctionType 1. Or delete character from CharactersActivities if bFunctionType 0
void ASocialCell::ChangeCharacterWorkStatus(uint8 FunctionType, bool bDeleteItemFromCharactersArray, ATerraCharacter* LocalCharacter, UObject* ObjectForWorking, FName ActivityInfo, FName AddInfo)
{
	TArray<ATerraCharacter*> LocalCharactersArray;
	FName LocalActivityName;

	switch (FunctionType)
	{
	case 0:

		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityName", ActivityInfo);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityInfo", AddInfo);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsEnum("GlobalCharacterStatusKey", (uint8)ECharacterStatus::Working);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityInt", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditional", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditionalAnother", 0);
		
		LocalCharacter->Activity = ActivityInfo;

		if (ObjectForWorking)
		{
			UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsObject("ActivityObject", ObjectForWorking);
		}

		if (CharactersActivities.Contains(ActivityInfo))
		{
			LocalCharactersArray = *CharactersActivities.Find(ActivityInfo);
			LocalCharactersArray.Add(LocalCharacter);
			CharactersActivities.Add(ActivityInfo, LocalCharactersArray);
		}
		else
		{
			LocalCharactersArray.Add(LocalCharacter);
			CharactersActivities.Add(ActivityInfo, LocalCharactersArray);
		}



		if (bDeleteItemFromCharactersArray)
		{
			CharactersOutOfActivities.Remove(LocalCharacter);
		}

		break;

	case 1:

		if (bDeleteItemFromCharactersArray)
		{
			LocalActivityName = UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->GetValueAsName("ActivityName");

			LocalCharactersArray = *CharactersActivities.Find(LocalActivityName);
			LocalCharactersArray.Remove(LocalCharacter);
			CharactersActivities.Add(ActivityInfo, LocalCharactersArray);
		}

		LocalActivityName = "ReturnToSC";
		LocalCharacter->Activity = LocalActivityName;
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityName", LocalActivityName);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityInfo", AddInfo);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsEnum("GlobalCharacterStatusKey", (uint8)ECharacterStatus::Working);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityInt", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditional", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditionalAnother", 0);

		if (CharactersActivities.Contains(LocalActivityName))
		{
			LocalCharactersArray = *CharactersActivities.Find(LocalActivityName);
			LocalCharactersArray.Add(LocalCharacter);
			CharactersActivities.Add(LocalActivityName, LocalCharactersArray);
		}
		else
		{
			LocalCharactersArray.Add(LocalCharacter);
			CharactersActivities.Add(LocalActivityName, LocalCharactersArray);
		}

		break;

	case 2:

		if (bDeleteItemFromCharactersArray)
		{
			LocalActivityName = UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->GetValueAsName("ActivityName");

			if (CharactersActivities.Contains(LocalActivityName))
			{
				LocalCharactersArray = *CharactersActivities.Find(LocalActivityName);
				LocalCharactersArray.Remove(LocalCharacter);
				CharactersActivities.Add(ActivityInfo, LocalCharactersArray);
			}
		}

		LocalCharacter->Activity = "Rest";
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityName", "Rest");
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsName("ActivityInfo", AddInfo);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsEnum("GlobalCharacterStatusKey", (uint8)ECharacterStatus::Relax);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityInt", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditional", 0);
		UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->SetValueAsInt("ActivityIntAdditionalAnother", 0);

		CharactersOutOfActivities.Add(LocalCharacter);

		break;

	default:
		break;
	}
}

void ASocialCell::BeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor ,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<ATerraCharacter>())
	{
		ATerraCharacter* LocalCharacter;
		LocalCharacter = Cast<ATerraCharacter>(OtherActor);
		
		if (!LocalCharacter->IsPlayerControlled())
		{
			if (UAIBlueprintHelperLibrary::GetBlackboard(LocalCharacter)->GetValueAsName("ActivityName") == "ReturnToSC" && Characters.Contains(LocalCharacter))
			{
				MarksActor = UniteMarks(MarksActor, LocalCharacter->MarksActor);
				LocalCharacter->MarksActor = UniteMarks(LocalCharacter->MarksActor, MarksActor);

				if (IsCharacterHaveRequestItems(LocalCharacter))
				{
					ChangeCharacterWorkStatus(0, true, LocalCharacter, nullptr, "GiveRequestToSC");
				}
				else
				{
					ChangeCharacterWorkStatus(2, true, LocalCharacter);  
				}
			}
		}
		
	}
}

//////////////////////

/// Searching ///

ATerraCharacter* ASocialCell::FindBetterCharacterForActivity(FName Activity, FName AdditionalInfo)
{
	ATerraCharacter* OutputCharacter;
	OutputCharacter = Characters[0];
	TMap<FName, float> ShareAttributesPriority;

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
			LocalValue = 0.0f;
			for (auto& LocalPriority : ShareAttributesPriority)
			{
				LocalValue += LocalCharacter->ProgressionComponent->FindAttribute(LocalPriority.Key)->Level * LocalPriority.Value;
			}

			if (LocalValue > PREVValue)
			{
				PREVValue = LocalValue;
				OutputCharacter = LocalCharacter;
			}
		}
	}
	else
	{
		FSActivitiesCreation* LocalActivitiesDataFromDT;
		LocalActivitiesDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(Activity, TEXT("none"), false);

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
		float LocalValue = 0.0f;
		float PREVValue = -1.0f;
		for (auto& LocalCharacter : Characters)
		{
			LocalValue = 0.0f;

			for (auto& LocalPriority : ShareAttributesPriority)
			{
				LocalValue += LocalCharacter->ProgressionComponent->FindAttribute(LocalPriority.Key)->Level * LocalPriority.Value;
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

FName ASocialCell::FindActivityByMark(TEnumAsByte<EMarkType> MarkType)
{
	TArray<FName> LocalNameArray = DT_Acitivites->GetRowNames();
	FName Output;

	for (auto& LocalActivity : LocalNameArray)
	{
		FSActivitiesCreation* ActivitiesDataFromDT;
		ActivitiesDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

		if (ActivitiesDataFromDT->MarkForActivities.Contains(MarkType))
		{
			Output = LocalActivity;

			break;
		}
	}

	return Output;
}

ATerraCharacter* ASocialCell::GetSlowestCharacter()
{
	float LocalValue = -1.0f;
	ATerraCharacter* OutputCharacter = nullptr;

	for (auto& LocalCharacter : Characters)
	{
		if (LocalCharacter->GetCharacterMovement()->MaxWalkSpeed > LocalValue)
		{
			LocalValue = LocalCharacter->GetCharacterMovement()->MaxWalkSpeed;
			OutputCharacter = LocalCharacter;
		}
	}

	return OutputCharacter;
}

bool ASocialCell::IsCharacterHaveRequestItems(ATerraCharacter* LocalCharacter)
{
	TMap<FName,float> LocalItemModifierBonuses;

	for (auto& SCRequestPair : RequestMap)
	{
		for (auto& LocalItem : LocalCharacter->InventoryComponent->Inventory)
		{
			LocalItemModifierBonuses = LocalCharacter->InventoryComponent->CalculateModifiersBasedOnDurability(LocalItem.Key);

			if (LocalItemModifierBonuses.Contains(SCRequestPair.Key))
			{
				if (*LocalItemModifierBonuses.Find(SCRequestPair.Key) > 0.0f)
				{
					return true;
				}
			}
		}
	}

	return false;
}

//TODO: ONLY FOR HERBS
//bFunctionType to Get Value based on: 0 - One Character, 1 - AllCharacters.
float ASocialCell::GetEfficencyMarkValue(AInteractionMark* LocalMark, bool bFunctionType, FName Status)
{
	float LocalValue = -10000.0f;
	float LocalGrowthStatusInSecond = -1.0f;
	float LocalDecayStatusInSecond = -1.0f;
	float LocalGrowthStatus = 0.0f;
	AInteractionMark* OutputMark = nullptr;
	
	FName LocalMarkID = LocalMark->ThisMarkActors[0]->InteractionComponent->AdditionalInfo;
	FSHerbCreation* HerbDataFromDT;
	FSItemCreation* ItemDataFromDT;

	TMap<FName, float> LocalModifierBonuses;

	float GrowthStatusInSecond = -1.0f;
	float DecayStatusInSecond = -1.0f;

	HerbDataFromDT = DT_Herb->FindRow<FSHerbCreation>(LocalMarkID, TEXT("none"), false);

	const float Time = GetSlowestCharacter()->CalculatePathDuration(this->GetActorLocation(), LocalMark->GetActorLocation());

	for (auto& LocalItemPair : HerbDataFromDT->ItemsGiven)
	{
		ItemDataFromDT = DT_Items->FindRow<FSItemCreation>(LocalItemPair.Key, TEXT("none"), false);
		LocalModifierBonuses = Characters[0]->InventoryComponent->CalculateItemModifiersDurabilityBasedOnSeconds(LocalItemPair.Key, Time);

		if (LocalModifierBonuses.Contains(Status))
		{
			LocalGrowthStatus += *LocalModifierBonuses.Find(Status) * LocalItemPair.Value;
		}
	}

	LocalGrowthStatusInSecond = LocalGrowthStatus / Time;
	LocalDecayStatusInSecond = ((GetStatusConsueAtDay(Status) + (Time * GetStatusConsumeAtSecond(Status))) * 1.25) / Characters[0]->TimeActor->DayLengthInRealSeconds;

	LocalValue = LocalGrowthStatusInSecond - LocalDecayStatusInSecond;
	GrowthStatusInSecond = LocalGrowthStatusInSecond;
	DecayStatusInSecond = LocalDecayStatusInSecond;
	

	if (!bFunctionType)
	{
		LocalValue += LocalGrowthStatusInSecond * (Characters.Num() - 1);
	}

	return LocalValue;
}

TArray<FName> ASocialCell::GetMarksIDForRestoreStatus(FName Status)
{
	FSStatusCreation* StatusDataFromDT;
	StatusDataFromDT = Characters[0]->StatusComponent->DT_Statuses->FindRow<FSStatusCreation>(Status, TEXT("none"), false);
	TArray<FName> LocalRows;
	
	TArray<FName> Output;
	float LocalValue;
	FString LocalString;

	for (auto& LocalActivity : StatusDataFromDT->ActivitiesToRestore)
	{
		FSActivitiesCreation* ActivityDataFromDT;
		ActivityDataFromDT = DT_Acitivites->FindRow<FSActivitiesCreation>(LocalActivity, TEXT("none"), false);

		for (auto& LocalMark : ActivityDataFromDT->MarkForActivities)
		{
			switch (LocalMark)
			{
			case EMarkType::Herbs:

				LocalRows = DT_Herb->GetRowNames();

				for (auto& LocalHerb : LocalRows)
				{
					FSHerbCreation* HerbDataFromDT;
					HerbDataFromDT = DT_Herb->FindRow<FSHerbCreation>(LocalHerb, TEXT("none"), false);

					for (auto& LocalItemPair : HerbDataFromDT->ItemsGiven)
					{
						if(Characters[0]->InventoryComponent->GetUnitedModifierBonusesByItemID(LocalItemPair.Key).Contains(Status))
						{
							LocalValue += *Characters[0]->InventoryComponent->GetUnitedModifierBonusesByItemID(LocalItemPair.Key).Find(Status);
						}
					}

					if (LocalValue > 0.0f)
					{
						LocalString.Append("Herbs;");
						LocalString.Append(LocalHerb.ToString());

						Output.Add(*LocalString);
					}
					LocalValue = 0.0f;
				}

				

				break;
			
			default:
				break;
			}
		}
	}

	return Output;
}

///

TMap<FName, TArray<AInteractionMark*>> ASocialCell::UniteMarks(TMap<FName, TArray<AInteractionMark*>> FirstMarksArray, TMap<FName, TArray<AInteractionMark*>> AnotherMarksArray)
{
	TArray<FName> LocalNamesArray;
	AnotherMarksArray.GenerateKeyArray(LocalNamesArray);

	TMap<FName, TArray<AInteractionMark*>> OutputArray;
	OutputArray = FirstMarksArray;

	TArray<AInteractionMark*> LocalIAArray;

	for (auto& LocalName : LocalNamesArray)
	{
		if (!FirstMarksArray.Contains(LocalName))
		{
			OutputArray.Add(LocalName, *AnotherMarksArray.Find(LocalName));
		}
		else
		{
			LocalIAArray = *OutputArray.Find(LocalName);
			for (auto& LocalIM : *AnotherMarksArray.Find(LocalName))
			{
				LocalIAArray.AddUnique(LocalIM);
			}
			OutputArray.Add(LocalName, LocalIAArray);
		}
	}

	return OutputArray;
}




