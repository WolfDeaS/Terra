// Fill out your copyright notice in the Description page of Project Settings.

#include "EnvQueryContext_NearCharByActiv.h"
#include "TerraCharacter.h"
#include "SocialCell.h"


void UEnvQueryContext_NearCharByActiv::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	ATerraCharacter* Character = Cast<ATerraCharacter>((QueryInstance.Owner).Get());
	
	AActor* LocalActor = Cast<AActor>(Character);
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, LocalActor);
	/*
	if ((QueryInstance.Owner).Get())
	{
		if (Character->SocialCell)
		{
			
			
			for (auto& LocalCharacter : *Character->SocialCell->CharactersActivities.Find(Character->Activities))
			{
				if (LocalCharacter != Character)
				{
					UEnvQueryItemType_Actor::SetContextHelper(ContextData, LocalCharacter);
				}
			}
		}
	}*/
}