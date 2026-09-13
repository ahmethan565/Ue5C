// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GeometryCollection/GeometryCollectionComponent.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BreakableCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(BreakableCollection);
	BreakableCollection->SetGenerateOverlapEvents(true);
	BreakableCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	
}

