// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CharacterStates.h"
#include "BaseCharacter.h"
#include "ASlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class AItem;
class UAnimMontage;

UCLASS()
class UE5C_API AASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SlashContext;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	
	virtual void AttackEnd() override;

	/* Play Montage Funcs */
	virtual void PlayAttackMontage() override;
	void PlayEquipMontage(FName SectionName);
	
private:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/**
	 * Anim Montages
	 */

	UPROPERTY(EditAnywhere, Category = AnimMontages)
	UAnimMontage* EquipAnimMontage;

	

	ECharacterStates CharacterState = ECharacterStates::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, Category = AnimMontages, meta = (AllowPrivateAccess = "true"))
	EActionStates ActionState = EActionStates::EAS_Unoccupied;

	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();


public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterStates GetCharacterState() const { return CharacterState; }

	
};
