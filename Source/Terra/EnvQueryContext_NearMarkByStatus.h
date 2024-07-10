// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "StatusComponent.h"
#include "SocialCell.h"
#include "InteractionMark.h"
#include "Kismet/GameplayStatics.h"
#include "EnvQueryContext_NearMarkByStatus.generated.h"

/**
 * 
 */
UCLASS()
class TERRA_API UEnvQueryContext_NearMarkByStatus : public UEnvQueryContext
{
	GENERATED_BODY()
	
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};
