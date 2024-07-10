// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
/**
 * 
 */
class TERRA_API UActorsArray : public UObject
{
public:

	TArray<AActor*> Actors;
	TArray<AInteractableActor*> InteractableActors;
	TArray<int> Ibnvej;
};
