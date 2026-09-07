// Fill out your copyright notice in the Description page of Project Settings.


#include "ASlashCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DSP/Chorus.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Ue5C/Items/Item.h"
#include "Ue5C/Items/Weapon.h"
#include "Animation/AnimMontage.h"


// Sets default values
AASlashCharacter::AASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(CameraBoom);
}

void AASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}
}

void AASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionStates::EAS_Unoccupied)
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSwordSocket"));
		CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{
		if (CanDisarm())
		{
			GEngine->AddOnScreenDebugMessage(1, 20, FColor::Green, "1");

			ActionState = EActionStates::EAS_Equipping;
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterStates::ECS_Unequipped;
		}
		else if (CanArm())
		{
			GEngine->AddOnScreenDebugMessage(2, 20, FColor::Green, "2");

			ActionState = EActionStates::EAS_Equipping;
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterStates::ECS_EquippedOneHandedWeapon;
		}
	}
}

void AASlashCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionStates::EAS_Attacking;
	}
}

bool AASlashCharacter::CanAttack()
{
	return (ActionState == EActionStates::EAS_Unoccupied &&
		CharacterState != ECharacterStates::ECS_Unequipped);
}

bool AASlashCharacter::CanDisarm()
{
	return (ActionState == EActionStates::EAS_Unoccupied &&
		CharacterState != ECharacterStates::ECS_Unequipped);
}

bool AASlashCharacter::CanArm()
{
	return (ActionState == EActionStates::EAS_Unoccupied &&
		CharacterState == ECharacterStates::ECS_Unequipped &&
		EquippedWeapon);
}

void AASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachWeapon(GetMesh(), FName("SocketArm"));
	}
}

void AASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachWeapon(GetMesh(), FName("RightHandSwordSocket"));
	}
}

void AASlashCharacter::FinishEquipping()
{
	ActionState = EActionStates::EAS_Unoccupied;
}

void AASlashCharacter::SetWeaponBoxCollision(ECollisionEnabled::Type CollisionType)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void AASlashCharacter::AttackEnd()
{
	ActionState = EActionStates::EAS_Unoccupied;
}

void AASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackingAnimMontage)
	{
		AnimInstance->Montage_Play(AttackingAnimMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackingAnimMontage);
	}
}

void AASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipAnimMontage)
	{
		AnimInstance->Montage_Play(EquipAnimMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipAnimMontage);
	}
}

void AASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AASlashCharacter::Attack);
	}
}
