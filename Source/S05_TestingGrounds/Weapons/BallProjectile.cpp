// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BallProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

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
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	ApplyPointDamage();

}

void ASBallProjectile::ApplyPointDamage()
{
	AActor* Mannequin = GetOwner();
	if (Mannequin)
	{
		float ActualDamage = BaseDamage;
		FHitResult Hit;
		FVector EyeLocation;
		FRotator EyeRotation;
		FVector ShotDirection = EyeRotation.Vector();
		UGameplayStatics::ApplyPointDamage(Mannequin, BaseDamage, ShotDirection, Hit, this->GetInstigatorController(), this, DamageType);
		UE_LOG(LogTemp, Warning, TEXT("DONKEY1: Damaged applied: %f"), BaseDamage);

		if (ActualDamage == 0)
		{
			Destroy(Mannequin);
		}

	}
}

