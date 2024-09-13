// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableRiver.h"

AInteractableRiver::AInteractableRiver()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(FName("InteractionComponent"));
	InteractionComponent->RegisterComponent();
	InteractionComponent->ActorRiverREF = this;

	GlobalSplineMeshComponent = CreateDefaultSubobject<USplineMeshComponent>(FName("GlobalSplineMeshComponent"));
	GlobalSplineMeshComponent->RegisterComponent();
	
}

void AInteractableRiver::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UE_LOG(LogTemp, Error, TEXT("Num of spline point %d"), GetWaterSpline()->GetNumberOfSplinePoints());

	for (size_t i = 0; i < (GetWaterSpline()->GetNumberOfSplinePoints() - 1); i++)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());


		SplineMeshComponent->SetMobility(EComponentMobility::Type::Movable);
		SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(GetWaterSpline(), FAttachmentTransformRules::KeepRelativeTransform);

		const FVector StartPoint = GetWaterSpline()->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = GetWaterSpline()->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = GetWaterSpline()->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = GetWaterSpline()->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

		SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SplineMeshComponent->SetCollisionProfileName("BlockAll", true);
	}
}