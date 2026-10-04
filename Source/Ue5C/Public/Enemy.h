// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/HitInterface.h"
#include "Ue5C/Characters/CharacterStates.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class UE5C_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void CheckPatrolTarget();
	
	void CheckCombatTarget();
	

protected:
	virtual void BeginPlay() override;

	void Die();

	bool InTargetRange(AActor* Target, double Radius);

	/**
	* Play Montage Sections
	*/

	void PlayHitReactMontage(const FName& SectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	void MoveToTarget(AActor* Target);

	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:
	/**
	* Anim Montages
	*/
	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* DieMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VFXs)
	UParticleSystem* HitVFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBar;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/**
	 *Navigation
	 */

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 8.f;

	UPROPERTY(VisibleAnywhere, Category = "AI Navigation")
	EEnemyStates EnemyState = EEnemyStates::EES_Patrolling;
};
