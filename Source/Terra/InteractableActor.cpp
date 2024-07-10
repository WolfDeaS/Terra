// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableActor.h"
#include "InteractionComponent.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(FName("InteractionComponent"));
	InteractionComponent->RegisterComponent();

	LocalSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Local Scene Component"));
	RootComponent = LocalSceneComponent;
	
	LocalStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Local Static Mesh"));
	LocalStaticMesh->SetupAttachment(LocalSceneComponent);
	
	LocalCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName("Local Collision Mesh"));
	LocalCollisionBox->SetupAttachment(LocalSceneComponent);

	LocalStaticMesh->SetCanEverAffectNavigation(false);
	LocalCollisionBox->SetCanEverAffectNavigation(false);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(FName("Inventory Component"));
	InventoryComponent->RegisterComponent();
}

void AInteractableActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (LocalMeshREF)
	{
		LocalStaticMesh->SetStaticMesh(LocalMeshREF); 
	}
}

void AInteractableActor::UpdateMesh(UStaticMesh* NewMesh)
{
	if (NewMesh)
	{
		//LocalStaticMesh->SetStaticMesh(NewMesh);
	}
}

void AInteractableActor::InitItem(float Mass)
{
	if (UPrimitiveComponent* PrimitiveComponent = this->FindComponentByClass<UPrimitiveComponent>())
	{
		PrimitiveComponent->SetCollisionProfileName("InteractableItem");
		PrimitiveComponent->SetSimulatePhysics(true);
		PrimitiveComponent->SetMassOverrideInKg(NAME_None, Mass);
	}
}
// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractionComponent->InteractID == EInteractID::Chest)
	{
		InventoryComponent->DT_Items = DT_Items; 
		InventoryComponent->Init();

		InventoryComponent->CreateItem(FName("grain"), 4, 1);

		InteractionComponent->ActorREF = this;
	}
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableActor::DestroyIA()
{
	this->Destroy();
}

TMap<FSItem, int> AInteractableActor::GetInventory()
{
	TMap<FSItem, int> LocalWaffe;
	FSItem NoneItem;
	NoneItem.ID = "None";
	LocalWaffe.Add(NoneItem, 1);

	if (InteractionComponent->InteractID == EInteractID::Chest)
	{
		return InventoryComponent->Inventory;
	}

	return LocalWaffe;
}