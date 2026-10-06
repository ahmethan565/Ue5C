#include "Enemy.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Ue5C/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Ue5C/Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Ue5C/HUD/HealthBarComponent.h"
#include "Ue5C/Items/Weapon.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBar = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthBar->SetHealthBarPercentage(Attributes->GetHealthPercent());

	if (HealthBar)
		HealthBar->SetVisibility(false);

	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyStates::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalActor(Target);
	MoveReq.SetAcceptanceRadius(15.f);
	EnemyController->MoveTo(MoveReq);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num() - 1;
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, ValidTargets.Num() - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyStates::EES_Chasing) return;
	if (SeenPawn->ActorHasTag("SlashCharacter"))
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		CombatTarget = SeenPawn;
		if (EnemyState != EEnemyStates::EES_Attacking)
		{
			EnemyState = EEnemyStates::EES_Chasing;
			MoveToTarget(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen!, Chase Player"));
		}
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::Die()
{
	if (DieMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Dead2;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Dead1;

			break;
		default:
			break;
		}
		AnimInstance->Montage_Play(DieMontage);

		AnimInstance->Montage_JumpToSection(SectionName, DieMontage);
		SetLifeSpan(3.f);
	}

	if (HealthBar)
		HealthBar->SetVisibility(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	// DRAW_SPHERE_COLOR(ImpactPoint, FColor::Cyan);

	if (Attributes && Attributes->bIsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint);
	}

	if (HitVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitVFX,
			ImpactPoint);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
	if (Attributes && HealthBar)
	{
		Attributes->ReceiveDamage(DamageAmount);

		HealthBar->SetHealthBarPercentage(Attributes->GetHealthPercent());

		HealthBar->SetVisibility(true);
	}

	CombatTarget = EventInstigator->GetPawn();

	EnemyState = EEnemyStates::EES_Chasing;
	MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	return DamageAmount;
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (CombatTarget)
	{
		if (!InTargetRange(CombatTarget, CombatRadius))
		{
			CombatTarget = nullptr;

			if (HealthBar)
			{
				HealthBar->SetVisibility(false);
			}

			EnemyState = EEnemyStates::EES_Patrolling;
			GetCharacterMovement()->MaxWalkSpeed = 150.f;
			MoveToTarget(PatrolTarget);

			UE_LOG(LogTemp, Warning, TEXT("Combat Target Lost Back to Patrolling"));
		}

		else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyStates::EES_Chasing)
		{
			//outside attack range chase player
			EnemyState = EEnemyStates::EES_Chasing;
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
			MoveToTarget(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
		}

		else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyStates::EES_Attacking)
		{
			EnemyState = EEnemyStates::EES_Attacking;
			//TODO AttackMontage
			UE_LOG(LogTemp, Warning, TEXT("Attack"));
		}
	}
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}
