// Fill out your copyright notice in the Description page of Project Settings.

#include "Voltigeur.h"
#include "Weapon.h"
#include "VoltigeurCharacterBase.h"
#include "Engine.h"

// Sets default values
AWeapon::AWeapon()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachTo(RootComponent);
}

void AWeapon::Fire()
{
	/*Depending on which bullet type,*/
	//if it is a regular bullet
	if (ProjectileType == EWeaponProjectile::EBullet)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Bullet"));
		Instant_Fire();
	}
	else if (ProjectileType == EWeaponProjectile::ESpread) //spread like shotgun
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Spread"));
		/*The reason why we use a for loop and then call instant fire
		*instead of just calling many instant fires is because, the function
		*instant fire will have a random seed embedded which will yield
		*different direction each time it is called*/
		for (int32 i = 0; i <= WeaponConfig.WeaponSpread; i++)
		{
			Instant_Fire();
		}
	}
	else if (ProjectileType == EWeaponProjectile::EProjectile)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, TEXT("Projectile"));
		ProjectileFire();
	}
}

void AWeapon::Instant_Fire()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = WeaponConfig.WeaponSpread;

	//multiply by 0.5 so that we get half of the circle
	const float SpreadCone = FMath::DegreesToRadians(WeaponConfig.WeaponSpread * 0.5f);

	//Get Socket Rotation yields FVector, need to get constant vector that won't move
	const FVector AimDir = WeaponMesh->GetSocketRotation("MF").Vector();
	const FVector StartTrace = WeaponMesh->GetSocketLocation("MF");

	/*Clamp shoot-cone: Want horizontal half of cone and vertical half of cone*/
	//@param AimDir where bullet is coming and going to
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, SpreadCone, SpreadCone);

	//Calculate distance from start direction and weapon range
	const FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
}

//@param need collision query paramater: e.g. ignore particular actor when tracing? Appear in scene? etc..
const FHitResult AWeapon::WeaponTrace(const FVector &TraceFrom, const FVector &TraceTo)
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator); //Instigator is who is firing the trace
																		//Want trace to be synced with the scene?
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	//Don't want interference from this weapon object when tracing
	TraceParams.AddIgnoredActor(this);

	//we need to reinitialize a new line every time we fire the weapon or line will spawn at same old loc
	FHitResult Hit(ForceInit);

	//Created game trace channel in .h file
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, TRACE_WEAPON, TraceParams);

	return Hit;
}

/*NOTE: DefaultEngine.ini +DefaultChannelResponse so that engine knows we are using our own trace channel*/
void AWeapon::ProcessInstantHit(const FHitResult &Impact, const FVector &Origin, const FVector &ShootDir, int32 RandomSeed, float ReticleSpread)
{
	//Make sure updates
	const FVector EndTrace = Origin + ShootDir * WeaponConfig.WeaponRange;
	//if we hit an actor, end the point there, if not keep tracing until end of trace
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	//Trace WeaponFire lines
	DrawDebugLine(this->GetWorld(), Origin, Impact.TraceEnd, FColor::Red, true, 10000, 10.f);

	/*Process enemy hit*/
	AVoltigeurCharacterBase *Enemy = Cast<AVoltigeurCharacterBase>(Impact.GetActor());
	if (Enemy && Enemy->GetFriendlyState() == EFriendlyState::EHostile)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "You Hit an ENEMY!");
		Enemy->Destroy(); //he dead
	}
}

void AWeapon::ProjectileFire()
{
	//implemented in Artillery class
}