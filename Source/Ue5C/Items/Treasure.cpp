// Fill out your copyright notice in the Description page of Project Settings.


#include "Treasure.h"
#include "Ue5C/Characters/ASlashCharacter.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	ItemState = EItemState::EIS_Hovering;
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AASlashCharacter* SlashCharacter = Cast<AASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				PickupSound,
				GetActorLocation()
				);
		}
		Destroy();
	}
}
