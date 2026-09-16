// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Treasure.h"
#include "Components/CapsuleComponent.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BreakableCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(BreakableCollection);
	BreakableCollection->SetGenerateOverlapEvents(true);
	BreakableCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BreakableCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
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
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		FVector Location = GetActorLocation();
		Location.Z = Location.Z + 120;
		World->SpawnActor<ATreasure>(
			TreasureClasses[Selection],
			Location,
			GetActorRotation());
	}
}

