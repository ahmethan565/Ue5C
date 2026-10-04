// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class UE5C_API ABaseCharacter : public ACharacter , public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollision(ECollisionEnabled::Type CollisionType);

	virtual void Attack();
	
	virtual bool CanAttack();

	virtual void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	virtual void Die();

	void PlayHitReactMontage(const FName& SectionName);

	void DirectionalHitReact(const FVector& ImpactPoint);
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* AttackingAnimMontage;

	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* DieMontage;

	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VFXs)
	UParticleSystem* HitVFX;
};
