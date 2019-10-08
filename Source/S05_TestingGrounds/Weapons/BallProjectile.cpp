// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BallProjectile.h"
#include "Gun.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ASBallProjectile::ASBallProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASBallProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	BaseDamage = 20.f;
}

void ASBallProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* Mannequin, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((Mannequin != NULL) && (Mannequin != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Warning, TEXT("DONKEY1: AddImpulse"));

		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		//Destroy();
	
			const FVector SpawnLocation = StaticMesh->GetComponentLocation();
			const FRotator SpawnRotation = StaticMesh->GetComponentRotation();

			float ActualDamage = BaseDamage;
			FHitResult HitResult;
			/*FVector EyeLocation;
			FRotator EyeRotation;*/
			FVector ShotDirection = SpawnRotation.Vector();
			FVector TraceEnd = SpawnLocation + (ShotDirection * 10000);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;



			if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, TraceEnd, ECC_EngineTraceChannel3, QueryParams))
			{
				DrawDebugLine(GetWorld(), SpawnLocation, TraceEnd, FColor::Red, 1, 1.f, 0, 1.f);
				UGameplayStatics::ApplyPointDamage(Mannequin, BaseDamage, ShotDirection, HitResult, GetInstigatorController(), this, DamageType);
				UE_LOG(LogTemp, Warning, TEXT("DONKEY1: Damaged applied: %f"), BaseDamage);

			}
		
		

		


		
	}

	

}


