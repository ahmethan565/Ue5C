// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class UE5C_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sine Params")
	float RunningTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Params")
	float Amplitude = 0.25f;
	
	UFUNCTION(BlueprintPure, Category = "Sine Params")
	float TransformedSin();
	
	UFUNCTION(BlueprintPure, Category = "Sine Params")
	float TransformedCos();
	
	template<typename T>
	T Avg(T first, T second);
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Sets")
	bool IsDebugActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Sphere;

	EItemState ItemState = EItemState::EIS_Hovering;


public:	
	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float TimeConst = 5.f;
	

};

template <typename T>
T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
}
