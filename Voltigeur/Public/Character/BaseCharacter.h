// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/************************************************************************/
/* Forward Declarations                                                 */
/************************************************************************/
class AWeapon;
class ARangedWeapon;


/*State of character's friendlyness to Player. This will eventually determine
AI behaviors*/
UENUM(BlueprintType)
enum class EFriendlyState : uint8
{
	EPlayer			UMETA(DisplayName = "Player"),
	EFriendly		UMETA(DisplayName = "Friendly"),
	ENeutral		UMETA(DisplayName = "Neutral"),
	EHostile		UMETA(DisplayName = "Hostile")
};

/*Basic inventory struct for weapons*/
struct FWeaponSlot
{
	//const uint8 BAREHANDS = 0;
	const uint8 RIFLE = 0;
	const uint8 PISTOL = 1;
	const uint8 MELEE = 2; //blade or tools

	const uint8 INVENTORY_SIZE = 3;
	uint8 CurrentlyEquippedSlot;
};

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Character Status", meta = (AllowPrivateAccess = "true"))
	float health; //Character's health
	//TODO implement stat system
};

UCLASS()
class VOLTIGEUR_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Set Friendly State
	void SetFriendlyState(EFriendlyState NewState);
	// Getter for Friendly State
	FORCEINLINE	EFriendlyState GetFriendlyState() const { return CurrentFriendlyState; }

	//Target enemy unit
	void SetTarget(ACharacter* const Enemy);
	FORCEINLINE	ACharacter* GetTarget() const { return Target; }

	//interlocutor is one that this character is interacting with
	void SetInterlocutor(ACharacter* const character);
	FORCEINLINE ACharacter* GetInterlocutor() const { return Interlocutor; }

	//TODO Need to use generic Weapon
	//Current Weapon
	FORCEINLINE ARangedWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	/*****************Camera Settings*****************/
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	void SetupCameraSettings();

	/****Default Top-Down Settings****/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZOOM_INCREMENT = 70.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MAX_TOPDOWN_BOOM_LENGTH = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MIN_TOPDOWN_BOOM_LENGTH = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float DEFAULT_ARM_LENGTH = 900.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator DEFAULT_CAMERA_ROT = FRotator(-50.f, 0.f, 0.f);


	/****Default Third-Person Settings****/
	const float MIN_CAM_ROT_PITCH = 0;
	
	void ZoomIn();
	void ZoomOut();
	void ChangeView();


	/*****************Interaction Settings***************/
	UPROPERTY(VisibleAnywhere, Category = "Character Status", meta = (AllowPrivateAccess = "false"))
	ACharacter* Target; //the character that this unit is targeting
	
	UPROPERTY(VisibleAnywhere, Category = "Character Status", meta = (AllowPrivateAccess = "false"))
	ACharacter* Interlocutor; //the character this unit is interacting/talking to

	/******************Character Data Settings*****************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Status", meta = (AllowPrivateAccess = "true"))
	EFriendlyState CurrentFriendlyState; //hostile to player, etc...

	UPROPERTY(EditDefaultsOnly, Category = "Character Status")
	FCharacterData CharacterData;
	//TODO float EyeRange;

	/******************Collision Settings**********************/
	/*
	@param OtherActor if other actor touches this CollisionComp
	@param OtherComp seam component? I'm not sure...
	*/
	UFUNCTION()
	void OnCollision(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	void SetupCollisionSettings();

	//If an actor collides such as weapon trigger pickup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComp;


	/******************Weapon Interaction Settings*************/
	/*Equipments*/
	TArray<TSubclassOf<ARangedWeapon>> WeaponInventory; //index 0 is nullptr which is fist
	FWeaponSlot WeaponSlot; //struct to organize slot # enumerations
	class ARangedWeapon* CurrentWeapon; //initiated to NULL which means bare-hands 
	void SetupCharacterSettings();
	void EquipPistol();
	void EquipRifle();
	void EquipMelee();
	void Aim();


};