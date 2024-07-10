// Fill out your copyright notice in the Description page of Project Settings.


#include "WHUD.h"
#include "Blueprint/UserWidget.h"
#include "TerraCharacter.h"
#include "Kismet/GameplayStatics.h"

void UWHUD::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWHUD::Init()
{
	UpdateStatusPB();
}

void UWHUD::UpdateStatusPB()
{

	FString LocalString;
	UProgressBar* LocalPB;
	UTextBlock* LocalTB;
	float LocalValue;

	for (auto& LocalStatus : Statuses)
	{
		LocalString = "PB_";
		LocalString.Append(LocalStatus.ToString());

		
		LocalPB = Cast<UProgressBar>(this->WidgetTree->FindWidget(*LocalString));

		LocalString = LocalStatus.ToString();
		LocalString.Append("Max");
		
		LocalValue = *Character->Modifiers.Find(*LocalString);
		
		LocalPB->SetPercent(*Character->Modifiers.Find(LocalStatus) / LocalValue);

		LocalString = "TB_";
		LocalString.Append(LocalStatus.ToString());


		LocalTB = Cast<UTextBlock>(this->WidgetTree->FindWidget(*LocalString));

		if (LocalTB)
		{	
			FString OutputString;

			LocalString = LocalStatus.ToString();
			LocalString.Append("DebuffsEnd");

			OutputString.Append(FString::SanitizeFloat(*Character->Modifiers.Find(*LocalString)));
			OutputString.Append("/");
			OutputString.Append(FString::SanitizeFloat(*Character->Modifiers.Find(LocalStatus)));
			OutputString.Append("/");

			LocalString = LocalStatus.ToString();
			LocalString.Append("DebuffsStarts");
			OutputString.Append(FString::SanitizeFloat(*Character->Modifiers.Find(*LocalString)));

			LocalString = FString::SanitizeFloat(*Character->Modifiers.Find(*LocalString));
			LocalTB->SetText(FText::FromString(OutputString));
		}
	}
}