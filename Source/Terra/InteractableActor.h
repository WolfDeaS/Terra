// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionComponent.h"
#include "Components/BoxComponent.h"
#include "TerraCharacter.h"
#include "InteractableActor.generated.h"

UCLASS()
class TERRA_API AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

private:


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void UpdateMesh(UStaticMesh* NewMesh);
	void DestroyIA();
	TMap<FSItem, int> GetInventory();

	// Так як вони будуть динамічні, в плані падати і все таке, щоб все було тут.
	void InitItem(float Mass); 

	UPROPERTY(EditAnywhere)
	UStaticMesh* LocalMeshREF;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LocalStaticMesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* LocalSceneComponent;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LocalCollisionBox;

	UPROPERTY(EditAnywhere)
	class UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (RowType = "DT Items"));
	UDataTable* DT_Items;
};
