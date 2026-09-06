// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Ue5C/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Ue5C/Characters/ASlashCharacter.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConst);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConst);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	// const FString otherActorName = OtherActor->GetName();
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 20, FColor::Green, otherActorName);
	// }

	AASlashCharacter* SlashCharacter = Cast<AASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// const FString otherActorName = OtherActor->GetName();
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 20, FColor::Green, "end" + otherActorName);
	// }
	
	AASlashCharacter* SlashCharacter = Cast<AASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// float MovementRate = 50.f;
	// float RotationRate = 50.f;
	// 	
	// AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	// AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	//
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector Zero = FVector::ZeroVector;
	
	RunningTime += DeltaTime;

	FVector avgVector = Avg<FVector>(Location, Zero);

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f,0.f,TransformedSin()));
	}

	if (IsDebugActive)
	{
		DRAW_SPHERE_SingleFrame(Location);
		DRAW_VECTOR_SingleFrame(Location, Location + Forward * 100, FColor::Red);

		DRAW_POINT_SingleFrame(avgVector);
	}
	
	// float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConst);
	//
	// AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
}
