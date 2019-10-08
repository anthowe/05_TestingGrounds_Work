// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Character/Mannequin.h"
#include "Weapons/BallProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "MotionControllerComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));


}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();


}

void AGun::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ASBallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			
			FVector EyeLocation;
			FVector ShotDirection = SpawnRotation.Vector();
			FVector TraceEnd = SpawnLocation + (ShotDirection * 1000);
			FHitResult Hit;

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			//DrawDebugLine(GetWorld(), SpawnLocation, TraceEnd, FColor::Red, 1, 1.f, 0, 1.f);
			/*if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::ECC_EngineTraceChannel3, QueryParams))
			{
				float BaseDamage = 20.f;
				AActor* HitActor = Hit.GetActor();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), BaseDamage, SpawnLocation, Hit, this->GetInstigatorController(), this, DamageType);
				UE_LOG(LogTemp, Warning, TEXT("DONKEY: Damage applied: %f"), BaseDamage);
			}*/
		
	
		}
	}
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation1P != nullptr && AnimInstance1P !=nullptr)
		{
			AnimInstance1P->Montage_Play(FireAnimation1P, 1.f);
		}
	if (FireAnimation3P != nullptr && AnimInstance3P != nullptr)
	{
		AnimInstance3P->Montage_Play(FireAnimation3P, 1.f);
	}
	
}

void AGun::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	InputComponent->BindAction("Fire", IE_Pressed, this, &AGun::OnFire);
	
	
}
